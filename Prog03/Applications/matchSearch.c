// Programs/matchSearch.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define COMPARE_EXE "./ImageLibrary/executables/compare"

// ---------- tiny helpers ----------

// returns 1 if str ends with .tga or .TGA, else 0
static int has_tga_ext(const char* path) {
    size_t n = strlen(path);
    if (n < 4) return 0;
    const char* e = path + (n - 4);
    if (e[0] != '.') return 0;
    int is_t = (e[1] == 't' || e[1] == 'T');
    int is_g = (e[2] == 'g' || e[2] == 'G');
    int is_a = (e[3] == 'a' || e[3] == 'A');
    return (is_t && is_g && is_a) ? 1 : 0;
}

static void join_path(char* dst, size_t dstsz, const char* dir, const char* leaf) {
    size_t dl = strlen(dir);
    int need_slash = (dl > 0 && dir[dl-1] != '/');
    if (need_slash) snprintf(dst, dstsz, "%s/%s", dir, leaf);
    else            snprintf(dst, dstsz, "%s%s",  dir, leaf);
}

static const char* base_name(const char* path) {
    const char* s = strrchr(path, '/');
#ifdef _WIN32
    const char* b = strrchr(path, '\\');
    if (!s || (b && b > s)) s = b;
#endif
    return s ? (s + 1) : path;
}

static void build_report_path(char* dst, size_t dstsz, const char* outDir, const char* inputPath) {
    const char* base = base_name(inputPath);
    const char* dot  = strrchr(base, '.');
    size_t stem_len  = dot ? (size_t)(dot - base) : strlen(base);

    size_t od = strlen(outDir);
    int need_slash = (od == 0 || outDir[od-1] != '/');

    if (need_slash)
        snprintf(dst, dstsz, "%s/%.*s.txt", outDir, (int)stem_len, base);
    else
        snprintf(dst, dstsz, "%s%.*s.txt",  outDir, (int)stem_len, base);
}

// ---------- dynamic arrays for jobs/matches ----------

typedef struct {
    pid_t pid;
    char  cand_path[PATH_MAX];   // full path to candidate
    char  cand_name[NAME_MAX+1]; // just filename for report
} Job;

typedef struct {
    char** items;
    int    count;
    int    cap;
} StrVec;

static void sv_init(StrVec* v) { v->items=NULL; v->count=0; v->cap=0; }
static void sv_push(StrVec* v, const char* s) {
    if (v->count == v->cap) {
        int nc = (v->cap == 0) ? 8 : v->cap*2;
        char** ni = (char**)realloc(v->items, nc * sizeof(char*));
        if (!ni) return;
        v->items = ni; v->cap = nc;
    }
    v->items[v->count++] = strdup(s);
}
static void sv_free(StrVec* v) {
    for (int i=0; i<v->count; i++) free(v->items[i]);
    free(v->items);
}

// ---------- main ----------

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input.tga> <search_dir> <output_dir>\n", argv[0]);
        return 1;
    }

    const char* inputPath = argv[1];
    const char* searchDir = argv[2];
    const char* outputDir = argv[3];

    // Enumerate .tga files in searchDir
    DIR* d = opendir(searchDir);
    if (!d) {
        // Per spec, scripts handle folder validity; but fail clearly if not.
        fprintf(stderr, "Cannot open search dir: %s\n", searchDir);
        return 1;
    }

    // Collect jobs (one per candidate)
    Job* jobs = NULL;
    int   nj  = 0, cap = 0;

    struct dirent* ent;
    while ((ent = readdir(d)) != NULL) {
        // skip . and ..
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        // full path
        char full[PATH_MAX];
        join_path(full, sizeof(full), searchDir, ent->d_name);

        // regular file?
        struct stat st;
        if (stat(full, &st) != 0) continue;
        if (!S_ISREG(st.st_mode)) continue;

        if (!has_tga_ext(ent->d_name)) continue;

        // grow jobs
        if (nj == cap) {
            int nc = (cap == 0) ? 16 : cap * 2;
            Job* njobs = (Job*)realloc(jobs, nc * sizeof(Job));
            if (!njobs) { closedir(d); free(jobs); return 1; }
            jobs = njobs; cap = nc;
        }

        // store job
        jobs[nj].pid = -1;
        strncpy(jobs[nj].cand_path, full, sizeof(jobs[nj].cand_path)-1);
        jobs[nj].cand_path[sizeof(jobs[nj].cand_path)-1] = '\0';
        strncpy(jobs[nj].cand_name, ent->d_name, sizeof(jobs[nj].cand_name)-1);
        jobs[nj].cand_name[sizeof(jobs[nj].cand_name)-1] = '\0';
        nj++;
    }
    closedir(d);

    // Launch children
    for (int i = 0; i < nj; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            // fork failed: treat as no match; continue
            continue;
        }
        if (pid == 0) {
            // child: exec compare input vs candidate
            execl(COMPARE_EXE, "compare", inputPath, jobs[i].cand_path, (char*)NULL);
            // if exec fails, treat as "no match"
            _exit(0);
        }
        jobs[i].pid = pid;
    }

    // Collect results
    StrVec matches; sv_init(&matches);

    for (int i = 0; i < nj; i++) {
        if (jobs[i].pid <= 0) continue;
        int status = 0;
        pid_t done = waitpid(jobs[i].pid, &status, 0);
        if (done == jobs[i].pid && WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            if (code == 1) {
                sv_push(&matches, jobs[i].cand_name);
            }
        }
    }

    // Write report
    char reportPath[PATH_MAX];
    build_report_path(reportPath, sizeof(reportPath), outputDir, inputPath);

    FILE* out = fopen(reportPath, "w");
    if (!out) {
        fprintf(stderr, "Cannot write report: %s\n", reportPath);
        sv_free(&matches);
        free(jobs);
        return 1;
    }

    fprintf(out, "%s\n", inputPath);
    fprintf(out, "%s\n", searchDir);
    fprintf(out, "%d\n", matches.count);
    if (matches.count > 0) {
        //fputc(' ', out);
        for (int i=0; i<matches.count; i++) {
            if (i > 0) fputc(' ', out);
            fputs(matches.items[i], out);
        }
    }
    fputc('\n', out);
    fclose(out);

    sv_free(&matches);
    free(jobs);
    return 0;
}
