#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "report.h"

static void metricInit(struct metric *m, int kind)
{
    m->kind = kind;
    m->value = 0;
    m->lower_bound = 0;
    m->upper_bound = 0;
}

static void metricPrint(struct metric *m)
{
    const char *kind;
    switch (m->kind) {
        case METRIC_KIND_LATENCY:
            kind = "latency";
            break;
        case METRIC_KIND_THROUGHPUT:
            kind = "throughput";
            break;
        default:
            kind = NULL;
            assert(0);
            break;
    }

    printf("    \"%s\": {\n", kind);
    printf("      \"value\": %f,\n", m->value);
    printf("      \"lower_bound\": %f,\n", m->lower_bound);
    printf("      \"upper_bound\": %f\n", m->upper_bound);
    printf("    }");
}

static void benchmarkInit(struct benchmark *b, char *name)
{
    b->name = name;
    b->metrics = NULL;
    b->n_metrics = 0;
}

static void benchmarkClose(struct benchmark *b)
{
    unsigned i;

    for (i = 0; i < b->n_metrics; i++) {
        struct metric *metric = b->metrics[i];
        free(metric);
    }
    free(b->name);
    free(b->metrics);
}

struct metric *BenchmarkGrow(struct benchmark *b, int kind)
{
    struct metric *m = malloc(sizeof *m);
    unsigned n = b->n_metrics + 1;
    assert(m != NULL);

    b->metrics = realloc(b->metrics, n * sizeof *b->metrics);
    assert(b->metrics != NULL);

    b->metrics[n - 1] = m;
    b->n_metrics = n;

    metricInit(m, kind);

    return m;
}

static void benchmarkPrint(struct benchmark *b)
{
    unsigned i;

    printf("  \"%s\": {\n", b->name);

    for (i = 0; i < b->n_metrics; i++) {
        struct metric *metric = b->metrics[i];
        metricPrint(metric);
        if (i < b->n_metrics - 1) {
            printf(",");
        }
        printf("\n");
    }

    printf("  }");
}

void ReportInit(struct report *r)
{
    r->benchmarks = NULL;
    r->n_benchmarks = 0;
}

void ReportClose(struct report *r)
{
    unsigned i;

    for (i = 0; i < r->n_benchmarks; i++) {
        struct benchmark *benchmark = r->benchmarks[i];
        benchmarkClose(benchmark);
        free(benchmark);
    }

    free(r->benchmarks);
}

struct benchmark *ReportGrow(struct report *r, char *name)
{
    struct benchmark *b = malloc(sizeof *b);
    unsigned n = r->n_benchmarks + 1;

    assert(b != NULL);

    r->benchmarks = realloc(r->benchmarks, n * sizeof *r->benchmarks);
    assert(r->benchmarks != NULL);

    r->benchmarks[n - 1] = b;
    r->n_benchmarks = n;

    benchmarkInit(b, name);

    return b;
}

void ReportPrint(struct report *r)
{
    unsigned i;

    printf("{\n");

    for (i = 0; i < r->n_benchmarks; i++) {
        benchmarkPrint(r->benchmarks[i]);
        if (i < r->n_benchmarks - 1) {
            printf(",");
        }
        printf("\n");
    }

    printf("}\n");
}