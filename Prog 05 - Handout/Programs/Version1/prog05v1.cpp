// prog05v1.cpp
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sys/stat.h>
using namespace std;

struct Point {
    int r, c;
}; 

static bool dirExists(const string& path) {
    struct stat sb{};
    return stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode);
}

// base filename without extension
static string getBaseName(const string& path) {
    string basename = path;
    size_t lastSlash = basename.find_last_of("/\\");
    if (lastSlash != string::npos) basename = basename.substr(lastSlash + 1);
    size_t lastDot = basename.find_last_of('.');
    if (lastDot != string::npos) basename = basename.substr(0, lastDot);
    return basename;
}

// outDir + "/" + "<basename>.txt"
static string makeOutputPath(const string& outDir, const string& mapPath) {
    char sep = '/';
    string base = getBaseName(mapPath) + ".txt";
    if (!outDir.empty() && (outDir.back() == '/' || outDir.back() == '\\')) return outDir + base;
    return outDir + sep + base;
}

// 1-based bounds
static bool inBounds(int r, int c, int H, int W) {
    return r >= 1 && r <= H && c >= 1 && c <= W;
}

int main(int argc, char* argv[]) {

    bool trace = false;
    string mapPath, outDir;
    int startR = 0, startC = 0;

    // Args:
    //./prog05v1 <mapFile> <startRow> <startCol> <outputDir>
    //./prog05v1 -t <mapFile> <startRow> <startCol> <outputDir>
    if (argc == 5) {
        mapPath = argv[1];
        startR  = stoi(argv[2]);
        startC  = stoi(argv[3]);
        outDir  = argv[4];
    } else if (argc == 6 && string(argv[1]) == "-t") {
        trace   = true;
        mapPath = argv[2];
        startR  = stoi(argv[3]);
        startC  = stoi(argv[4]);
        outDir  = argv[5];
    } else {
        cerr << "Usage: " << argv[0] << " [-t] <mapFile> <startRow> <startCol> <outputDir>\n";
        return 1;
    }

    //Output folder missing exit 11
    if (!dirExists(outDir)) {
        return 11;
    }

    const string outputFile = makeOutputPath(outDir, mapPath);

    //Map file missing , write "File not found" to output file, exit 12
    {
        ifstream test(mapPath);
        if (!test) {
            ofstream fout(outputFile);
            fout << "File not found";
            return 12;
        }
    }

    // Open and read the map
    ifstream mapStream(mapPath);
    int H = 0, W = 0;
    mapStream >> H >> W;

    vector<vector<float>> mapData(H, vector<float>(W));
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            mapStream >> mapData[i][j];

    auto val = [&](int r1, int c1) -> float { return mapData[r1-1][c1-1]; };

    //Invalid start,write message, exit 13
    if (!inBounds(startR, startC, H, W)) {
        ofstream fout(outputFile);
        fout << "Start point at row=" << startR << ", column=" << startC << " is invalid.";
        return 13;
    }

    //Steepest descent
    const int dr[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    const int dc[8] = {-1, 0, 1,-1, 1,-1, 0, 1};

    vector<Point> path;
    Point cur{startR, startC};
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
        if (best.r == cur.r && best.c == cur.c) break; // local minimum
        cur = best;
        path.push_back(cur);
    }

    // Write output
    ofstream fout(outputFile);
    fout << (trace ? "TRACE\n" : "NO_TRACE\n");

    const Point& end = path.back();
    fout << startR << ' ' << startC << ' '
         << end.r  << ' ' << end.c  << ' '
         << path.size() << '\n';

    // TRACE: all points on one line, space-separated
    if (trace) {
        for (size_t i = 0; i < path.size(); ++i) {
            fout << path[i].r << ' ' << path[i].c;
            if (i + 1 < path.size()) fout << ' ';
        }
        fout << '\n';
    }
    return 0;
}
