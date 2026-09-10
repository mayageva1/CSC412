#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct Point {
    int r, c;
}; 

static bool dirExists(const std::string& path) {
    struct stat sb{};
    return stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode);
}
static int makeDirIfMissing(const std::string& path, mode_t mode = 0700) {
    if (dirExists(path)) return 0;
    return mkdir(path.c_str(), mode);
}
static std::string baseNoExt(const std::string& path) {
    std::string b = path;
    size_t s = b.find_last_of("/\\");
    if (s != std::string::npos) b = b.substr(s + 1);
    size_t d = b.find_last_of('.');
    if (d != std::string::npos) b = b.substr(0, d);
    return b;
}
static std::string joinPath(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    if (a.back() == '/' || a.back() == '\\') return a + b;
    return a + sep + b;
}
static bool inBounds(int r, int c, int H, int W) {
    return (r >= 1 && r <= H && c >= 1 && c <= W);
}


static void computePath(const std::vector<std::vector<float>>& grid,
                        int H, int W, int sr, int sc,
                        std::vector<Point>& path)
{
    const int dr[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    const int dc[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
    auto val = [&](int r, int c) { return grid[r-1][c-1]; };

    path.clear();
    Point cur{sr, sc};
    path.push_back(cur);

    while (true) {
        float curVal = val(cur.r, cur.c);
        float bestVal = curVal;
        Point best = cur;

        for (int k = 0; k < 8; ++k) {
            int nr = cur.r + dr[k];
            int nc = cur.c + dc[k];
            if (!inBounds(nr, nc, H, W)) continue;
            float nv = val(nr, nc);
            if (nv < bestVal) {
                bestVal = nv;
                best = {nr, nc};
            }
        }
        if (best.r == cur.r && best.c == cur.c) break; // local min
        cur = best;
        path.push_back(cur);
    }
}

// Child writes one simple .part file:
// INVALID r c
// or
// OK sr sc er ec len
// path line if trace: r1 c1 r2 c2 ..
static void writePartFile(const std::string& filePath,
                          bool trace, bool valid,
                          int sr, int sc,
                          const std::vector<Point>* pathPtr)
{
    std::ofstream out(filePath);
    if (!out) _exit(1);

    if (!valid) {
        out << "INVALID " << sr << " " << sc << "\n";
    } else {
        const auto& path = *pathPtr;
        const Point& end = path.back();
        out << "OK " << sr << " " << sc << " "
            << end.r << " " << end.c << " " << path.size() << "\n";
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

int main(int argc, char* argv[]) {
    using std::string; using std::vector; using std::ifstream; using std::ofstream;

    // ---- Parse arguments ----
    // [-t] <mapFile> <r1> <c1> [<r2> <c2> ...] <outputDir>
    bool trace = false;
    int idx = 1;
    if (idx < argc && string(argv[idx]) == "-t") { trace = true; ++idx; }


    string mapPath = argv[idx++];

    // The last argument is the output directory
    string outDir = argv[argc - 1];

    // Start pairs are everything from idx to argc-2 (inclusive)
    int tokens = (argc - 1) - idx; // exclude outDir
    int nStarts = tokens / 2;

    // 1) Output folder missing -> exit 11 (no file)
    if (!dirExists(outDir)) return 11;

    // Final output file path
    const string outFile = joinPath(outDir, baseNoExt(mapPath) + ".txt");

    //Map file missing, write "File not found", exit 12
    {
        ifstream test(mapPath);
        if (!test) {
            ofstream fout(outFile);
            fout << "File not found";
            return 12;
        }
    }

    // Read map once in parent 
    ifstream mapIn(mapPath);
    int H = 0, W = 0;
    mapIn >> H >> W;

    vector<vector<float>> grid(H, vector<float>(W));
    for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c)
            mapIn >> grid[r][c];
    mapIn.close();

    // Collect starts (keep order)
    vector<Point> starts;
    starts.reserve(nStarts);
    for (int i = 0; i < nStarts; ++i) {
        int sr = std::stoi(argv[idx + 2*i]);
        int sc = std::stoi(argv[idx + 2*i + 1]);
        starts.push_back({sr, sc});
    }

    //Make a simple temp folder inside outDir
    // One per run using parent PID avoids collisions between runs.
    char tmpName[64];
    std::snprintf(tmpName, sizeof(tmpName), ".tmp_prog05v2_%d", (int)getpid());
    const string tmpDir = joinPath(outDir, tmpName);
    makeDirIfMissing(tmpDir, 0700);

    //Fork children (one per start)
    vector<pid_t> kids(nStarts, -1);
    for (int i = 0; i < nStarts; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            // Fork failed: synthesize an INVALID part file
            const string partPath = joinPath(tmpDir, "part_" + std::to_string(i) + ".part");
            ofstream o(partPath);
            if (o) o << "INVALID " << starts[i].r << " " << starts[i].c << "\n";
            continue;
        }
        if (pid == 0) {
            //Child
            const string partPath = joinPath(tmpDir, "part_" + std::to_string(i) + ".part");
            if (!inBounds(starts[i].r, starts[i].c, H, W)) {
                writePartFile(partPath, trace, /*valid=*/false,
                              starts[i].r, starts[i].c, nullptr);
                _exit(0);
            } else {
                vector<Point> path;
                computePath(grid, H, W, starts[i].r, starts[i].c, path);
                writePartFile(partPath, trace, /*valid=*/true,
                              starts[i].r, starts[i].c, &path);
                _exit(0);
            }
        } else {
            kids[i] = pid; //parent tracks child
        }
    }

    //Parent waits for all children 
    for (pid_t pid : kids) {
        if (pid > 0) {
            int status = 0;
            (void)waitpid(pid, &status, 0);
        }
    }

    //  Aggregate part files in order and write final output
    std::ofstream out(outFile);
    out << (trace ? "TRACE\n" : "NO_TRACE\n");
    out << nStarts << "\n";

    for (int i = 0; i < nStarts; ++i) {
        const string partPath = joinPath(tmpDir, "part_" + std::to_string(i) + ".part");
        std::ifstream pf(partPath);
        if (!pf) {
            // Missing part -> treat as invalid
            out << "Start point at row=" << starts[i].r
                << ", column=" << starts[i].c << " is invalid\n";
            continue;
        }

        std::string tag;
        pf >> tag;
        if (tag == "INVALID") {
            int sr, sc;
            pf >> sr >> sc;
            out << "Start point at row=" << sr << ", column=" << sc << " is invalid\n";
        } else if (tag == "OK") {
            int sr, sc, er, ec;
            size_t len;
            pf >> sr >> sc >> er >> ec >> len;
            out << sr << " " << sc << " " << er << " " << ec << " " << len << "\n";
            if (trace) {
                // Echo the path line exactly as one line of "r c" pairs
                std::vector<int> coords;
                coords.reserve(2 * len);
                for (size_t k = 0; k < 2 * len; ++k) {
                    int v; pf >> v; coords.push_back(v);
                }
                for (size_t k = 0; k < coords.size(); ++k) {
                    out << coords[k] << (k + 1 < coords.size() ? " " : "");
                }
                out << "\n";
            }
        } else {
            // Unknown, invalid
            out << "Start point at row=" << starts[i].r
                << ", column=" << starts[i].c << " is invalid\n";
        }

        pf.close();
        std::remove(partPath.c_str()); // delete part file
    }

    out.close();
    rmdir(tmpDir.c_str()); // remove temp folder 
    return 0;
}
