// Build:  g++ -std=gnu++20 -O2 -Wall -Wextra -o prog05v4 prog05v4.cpp
// Role:   General dispatcher (Version 4, no extra credit)
// Works with script05.sh which feeds this program paths via stdin.
// Protocol (one line per message from the script):
//   <absolute-or-relative-path>.map      -> launch (or reuse) skier-dispatcher for this map
//   <absolute-or-relative-path>.task     -> forward to the right skier-dispatcher
// End-of-file on stdin                   -> send QUIT to all children, wait, exit.

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>          // map<string, Child> (portable on mac)
#include <cstdio>
#include <cstdlib>
#include <cctype>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

// ---------------------------------------------------------
// Tiny helpers (avoid <filesystem> to keep it portable here)
// ---------------------------------------------------------
static bool dirExists(const string& path) {
    struct stat sb{};
    return ::stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode);
}

static string baseNoExt(const string& path) {
    // "foo/bar/smallMap_Vert.map" -> "smallMap_Vert"
    string b = path;
    size_t s = b.find_last_of("/\\");
    if (s != string::npos) b = b.substr(s + 1);
    size_t d = b.find_last_of('.');
    if (d != string::npos) b = b.substr(0, d);
    return b;
}

static string joinPath(const string& a, const string& b) {
    if (a.empty()) return b;
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    if (a.back() == '/' || a.back() == '\\') return a + b;
    return a + sep + b;
}

static bool endsWith(const string& s, const string& suf) {
    return s.size() >= suf.size() &&
           s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

static string trim(const string& s) {
    size_t i = 0, j = s.size();
    while (i < j && isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && isspace(static_cast<unsigned char>(s[j - 1]))) --j;
    return s.substr(i, j - i);
}

static bool inBounds(int r, int c, int H, int W) {
    return r >= 1 && r <= H && c >= 1 && c <= W;
}

// ---------------------------------------------------------
// Steepest descent core used by a skier dispatcher (child)
// ---------------------------------------------------------
struct Point { int r, c; };

static void computePath(const vector<vector<float>>& grid,
                        int H, int W, int sr, int sc,
                        vector<Point>& out) {
    static const int dr[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    static const int dc[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
    auto val = [&](int r,int c){ return grid[r-1][c-1]; };

    out.clear();
    Point cur{sr, sc};
    out.push_back(cur);

    while (true) {
        float curVal = val(cur.r, cur.c);
        float bestVal = curVal;
        Point best = cur;

        for (int k = 0; k < 8; ++k) {
            int nr = cur.r + dr[k], nc = cur.c + dc[k];
            if (!inBounds(nr, nc, H, W)) continue;
            float nv = val(nr, nc);
            if (nv < bestVal) { bestVal = nv; best = {nr, nc}; }
        }
        if (best.r == cur.r && best.c == cur.c) break; // local min
        cur = best;
        out.push_back(cur);
    }
}

// ---------------------------------------------------------
// Skier dispatcher process (child)
// Parent talks to child via pipe connected to child's stdin.
// Child protocol (lines from parent):
//   MAP <mapPath>
//   TASK <taskPath>
//   QUIT
// Task file format (simple & tolerant):
//   First non-empty token may be TRACE or -t (enables trace)
//   Then any number of integers read as r c pairs (both 1-based).
// Output file name: <outDir>/<basename(map)>.txt
// ---------------------------------------------------------
static void skier_dispatcher_run(int read_fd, const string& outDir) {
    FILE* fp = fdopen(read_fd, "r");
    if (!fp) _exit(1);

    char lineBuf[4096];

    // Expect MAP line first
    string mapPath;
    if (!fgets(lineBuf, sizeof(lineBuf), fp)) _exit(1);
    {
        string L = trim(lineBuf);
        if (L.rfind("MAP ", 0) != 0) _exit(1);
        mapPath = trim(L.substr(4));
    }

    const string outFile = joinPath(outDir, baseNoExt(mapPath) + ".txt");

    // Load map once in the child
    bool mapOK = false;
    int H = 0, W = 0;
    vector<vector<float>> grid;

    {
        ifstream test(mapPath);
        if (test) {
            test >> H >> W;
            if (H > 0 && W > 0) {
                grid.assign(H, vector<float>(W));
                for (int r = 0; r < H; ++r)
                    for (int c = 0; c < W; ++c)
                        test >> grid[r][c];
                mapOK = true;
            }
        }
    }

    // Handle TASK/QUIT lines
    while (fgets(lineBuf, sizeof(lineBuf), fp)) {
        string L = trim(lineBuf);
        if (L.empty()) continue;
        if (L == "QUIT") break;
        if (L.rfind("TASK ", 0) != 0) continue;
        string taskPath = trim(L.substr(5));

        // Parse the task file:
        // - If first token is TRACE or -t -> trace mode
        // - Then parse all remaining integers as (r c) pairs
        bool trace = false;
        vector<pair<int,int>> starts;
        {
            ifstream tin(taskPath);
            string tok;
            vector<int> nums;
            while (tin >> tok) {
                string tU = tok;
                for (char& ch : tU) ch = std::toupper(static_cast<unsigned char>(ch));
                if (tU == "TRACE" || tok == "-t") {
                    trace = true;
                    continue;
                }
                char* end = nullptr;
                long v = strtol(tok.c_str(), &end, 10);
                if (end && *end == '\0') nums.push_back((int)v);
                // else ignore junk tokens gracefully
            }
            for (size_t i = 0; i + 1 < nums.size(); i += 2) {
                starts.emplace_back(nums[i], nums[i + 1]);
            }
        }
        if (starts.empty()) {
            // nothing to do for this task
            continue;
        }

        ofstream out(outFile);
        out << (trace ? "TRACE\n" : "NO_TRACE\n");
        out << (int)starts.size() << "\n";

        if (!mapOK) {
            out << "File not found";
            out.close();
            continue;
        }

        for (auto [sr, sc] : starts) {
            if (!inBounds(sr, sc, H, W)) {
                out << "Start point at row=" << sr
                    << ", column=" << sc << " is invalid\n";
                continue;
            }
            vector<Point> path;
            computePath(grid, H, W, sr, sc, path);
            const Point& end = path.back();
            out << sr << " " << sc << " "
                << end.r << " " << end.c << " "
                << path.size() << "\n";
            if (trace) {
                for (size_t i = 0; i < path.size(); ++i) {
                    out << path[i].r << " " << path[i].c;
                    if (i + 1 < path.size()) out << " ";
                }
                out << "\n";
            }
        }
        out.close();
    }

    fclose(fp);
    _exit(0);
}

// ---------------------------------------------------------
// General dispatcher (this process)
// Reads stdin lines from the script:
//   *.map  -> start (or keep) a child for that map; send "MAP <path>"
//   *.task -> forward "TASK <path>" to the matching child by map base name
// On EOF  -> send "QUIT" to all children, close pipes, wait, exit.
// ---------------------------------------------------------
struct Child {
    pid_t pid  = -1;
    int   wfd  = -1;  // parent's write-end to child's stdin
};

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <outputDir>\n";
        return 1;
    }
    string outDir = argv[1];
    if (!dirExists(outDir)) return 11; // per spec, no file output, just exit code

    // map base (e.g., "smallMap_Vert") -> child
    map<string, Child> children;

    string path;
    while (true) {
        if (!std::getline(cin, path)) break; // EOF from the script
        path = trim(path);
        if (path.empty()) continue;

        if (endsWith(path, ".map")) {
            string base = baseNoExt(path);

            // already have a child for this map? ignore duplicate
            if (children.find(base) != children.end()) {
                continue;
            }

            // create pipe & fork a child (skier dispatcher)
            int fds[2];
            if (pipe(fds) != 0) {
                perror("pipe");
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                close(fds[0]); close(fds[1]);
                continue;
            }
            if (pid == 0) {
                // child: reads from fds[0]
                close(fds[1]);
                skier_dispatcher_run(fds[0], outDir);
                // never returns
            } else {
                // parent: writes to fds[1]
                close(fds[0]);
                children[base] = Child{pid, fds[1]};
                // send MAP line immediately
                string line = "MAP " + path + "\n";
                (void)write(fds[1], line.c_str(), line.size());
            }
        }
        else if (endsWith(path, ".task")) {
            // route task to the child whose map base matches the task base
            string tbase = baseNoExt(path);
            auto it = children.find(tbase);
            if (it == children.end()) {
                // unknown map (script should normally guard this)
                continue;
            }
            string line = "TASK " + path + "\n";
            (void)write(it->second.wfd, line.c_str(), line.size());
        }
        else {
            // ignore other files
            continue;
        }
    }

    // clean shutdown: tell all children to quit, close, wait
    for (auto &kv : children) {
        Child &ch = kv.second;
        if (ch.wfd >= 0) {
            string q = "QUIT\n";
            (void)write(ch.wfd, q.c_str(), q.size());
            close(ch.wfd);
        }
    }
    for (auto &kv : children) {
        Child &ch = kv.second;
        if (ch.pid > 0) {
            int st = 0;
            (void)waitpid(ch.pid, &st, 0);
        }
    }
    return 0;
}
