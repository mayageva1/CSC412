#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>

struct Point { 
    int r, c; 
}; // 1-based indices

static bool dirExists(const std::string& path) {
    struct stat sb{};
    return stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode);
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

//steepest descent 
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
            int nr = cur.r + dr[k], nc = cur.c + dc[k];
            if (!inBounds(nr, nc, H, W)) continue;
            float nv = val(nr, nc);
            if (nv < bestVal) { bestVal = nv; best = {nr, nc}; }
        }
        if (best.r == cur.r && best.c == cur.c) break;
        cur = best;
        path.push_back(cur);
    }
}

// Write the child’s result into the pipe as a small text protocol:
// INVALID r c
// or
// OK sr sc er ec len
// [r1 c1 r2 c2 ...]   (only if trace)
static void childWriteToPipe(int fd,
                             bool trace,
                             bool valid,
                             int sr, int sc,
                             const std::vector<Point>* pathPtr)
{
    std::string out;
    if (!valid) {
        out = "INVALID " + std::to_string(sr) + " " + std::to_string(sc) + "\n";
    } else {
        const auto& path = *pathPtr;
        const Point& end = path.back();
        out = "OK " + std::to_string(sr) + " " + std::to_string(sc) + " " +
              std::to_string(end.r) + " " + std::to_string(end.c) + " " +
              std::to_string(path.size()) + "\n";
        if (trace) {
            for (size_t i = 0; i < path.size(); ++i) {
                out += std::to_string(path[i].r) + " " + std::to_string(path[i].c);
                if (i + 1 < path.size()) out += " ";
            }
            out += "\n";
        }
    }
    // Write all bytes (small payloads so one write is fine)
    (void)write(fd, out.c_str(), out.size());
}

int main(int argc, char* argv[]) {
    using std::string; using std::vector; using std::ifstream; using std::ofstream;
    bool trace = false;
    int idx = 1;
    if (idx < argc && string(argv[idx]) == "-t") { trace = true; ++idx; }

    if (argc - idx < 4) {
        std::cerr << "Usage: " << argv[0]
                  << " [-t] <mapFile> <r1> <c1> [<r2> <c2> ...] <outputDir>\n";
        return 1;
    }

    string mapPath = argv[idx++];
    string outDir = argv[argc - 1];

    int tokens = (argc - 1) - idx; // exclude outDir
    if (tokens < 2 || tokens % 2 != 0) {
        std::cerr << "Provide start points as pairs of integers.\n";
        return 1;
    }
    int nStarts = tokens / 2;

    // Missing output dir, exit 11
    if (!dirExists(outDir)) return 11;

    // Final output file path
    const string outFile = joinPath(outDir, baseNoExt(mapPath) + ".txt");

    //Missing map file, write "File not found", exit 12
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

    // Collect starts
    vector<Point> starts;
    starts.reserve(nStarts);
    for (int i = 0; i < nStarts; ++i) {
        int sr = std::stoi(argv[idx + 2*i]);
        int sc = std::stoi(argv[idx + 2*i + 1]);
        starts.push_back({sr, sc});
    }

    // Create one pipe per child 
    struct PipeEnds { int rfd, wfd; }; // read-end, write-end
    vector<PipeEnds> pipes(nStarts);
    for (int i = 0; i < nStarts; ++i) {
        int fds[2];
        if (pipe(fds) != 0) {
            // If pipe creation fails, we’ll synthesize INVALID in parent later
            pipes[i] = {-1, -1};
        } else {
            pipes[i] = {fds[0], fds[1]};
        }
    }

    //  Fork one child per start 
    vector<pid_t> kids(nStarts, -1);
    for (int i = 0; i < nStarts; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            kids[i] = -1; // mark failure; parent will handle as invalid
            // Close any fds we opened for this index
            if (pipes[i].rfd >= 0) close(pipes[i].rfd);
            if (pipes[i].wfd >= 0) close(pipes[i].wfd);
            continue;
        }
        if (pid == 0) {
            // Child 
            // Close the read end; we only write
            if (pipes[i].rfd >= 0) close(pipes[i].rfd);

            if (!inBounds(starts[i].r, starts[i].c, H, W)) {
                if (pipes[i].wfd >= 0) {
                    childWriteToPipe(pipes[i].wfd, trace, /*valid=*/false,
                                     starts[i].r, starts[i].c, nullptr);
                    close(pipes[i].wfd);
                }
                _exit(0);
            } else {
                vector<Point> path;
                computePath(grid, H, W, starts[i].r, starts[i].c, path);
                if (pipes[i].wfd >= 0) {
                    childWriteToPipe(pipes[i].wfd, trace, /*valid=*/true,
                                     starts[i].r, starts[i].c, &path);
                    close(pipes[i].wfd);
                }
                _exit(0);
            }
        } else {
            // Parent 
            kids[i] = pid;
            // Parent will read; close write end later after reading (or immediately? keep it open for now)
            // But we should close our copy of the write-end to get EOF when child closes.
            if (pipes[i].wfd >= 0) close(pipes[i].wfd);
            // Keep pipes[i].rfd open for reading the child's message.
        }
    }

    // Parent waits for all children
    for (pid_t pid : kids) {
        if (pid > 0) {
            int status = 0;
            (void)waitpid(pid, &status, 0);
        }
    }

    //Open final output and write header
    std::ofstream out(outFile);
    out << (trace ? "TRACE\n" : "NO_TRACE\n");
    out << nStarts << "\n";

    // Read each child's pipe (in start order) and write the final lines
    for (int i = 0; i < nStarts; ++i) {
        std::string payload;

        if (pipes[i].rfd >= 0) {
            // Read all data until EOF
            char buf[256];
            ssize_t n;
            while ((n = read(pipes[i].rfd, buf, sizeof(buf))) > 0) {
                payload.append(buf, buf + n);
            }
            close(pipes[i].rfd);
        }

        if (payload.empty()) {
            // No data (pipe/child failure) -> treat as invalid per spec
            out << "Start point at row=" << starts[i].r
                << ", column=" << starts[i].c << " is invalid\n";
            continue;
        }

        // Parse the tiny protocol
        // We can use a stringstream since payload is tiny.
        std::stringstream ss(payload);
        std::string tag;
        ss >> tag;

        if (tag == "INVALID") {
            int sr, sc;
            ss >> sr >> sc;
            out << "Start point at row=" << sr << ", column=" << sc << " is invalid\n";
        } else if (tag == "OK") {
            int sr, sc, er, ec;
            size_t len = 0;
            ss >> sr >> sc >> er >> ec >> len;
            out << sr << " " << sc << " " << er << " " << ec << " " << len << "\n";
            if (trace) {
                // Next line is len pairs; echo as one line
                std::vector<int> coords;
                coords.reserve(2 * len);
                for (size_t k = 0; k < 2 * len; ++k) {
                    int v; ss >> v; coords.push_back(v);
                }
                for (size_t k = 0; k < coords.size(); ++k) {
                    out << coords[k] << (k + 1 < coords.size() ? " " : "");
                }
                out << "\n";
            }
        } else {
            // Unknown → treat as invalid (defensive, though shouldn't happen)
            out << "Start point at row=" << starts[i].r
                << ", column=" << starts[i].c << " is invalid\n";
        }
    }
    out.close();
    return 0;
}
