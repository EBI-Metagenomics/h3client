#include "h3client/result.h"
#include "h3client/rc.h"
#include "lite_pack/lite_pack.h"
#include "result.h"
#include "utils.h"
#include <stdlib.h>

struct h3c_result *h3c_result_new(void)
{
    struct h3c_result *result = malloc(sizeof(*result));
    if (!result) return 0;
    stats_init(&result->stats);
    tophits_init(&result->tophits);
    return result;
}

void h3c_result_del(struct h3c_result const *result)
{
    tophits_cleanup((struct tophits *)&result->tophits);
    free((void *)result);
}

enum h3c_rc h3c_result_pack(struct h3c_result const *result, FILE *file)
{
    struct lip_file f = {0};
    lip_file_init(&f, file);

    lip_write_map_size(&f, 1);
    lip_write_cstr(&f, "h3result");

    lip_write_map_size(&f, 2);
    lip_write_cstr(&f, "stats");
    enum h3c_rc rc = stats_pack(&result->stats, &f);
    if (rc) return rc;

    lip_write_cstr(&f, "tophits");
    return tophits_pack(&result->tophits, &f);
}

enum h3c_rc h3c_result_unpack(struct h3c_result *result, FILE *file)
{
    struct lip_file f = {0};
    lip_file_init(&f, file);

    if (!expect_map_size(&f, 1)) return H3C_FAILED_UNPACK;

    if (!expect_key(&f, "h3result")) return H3C_FAILED_UNPACK;

    if (!expect_map_size(&f, 2)) return H3C_FAILED_UNPACK;
    if (!expect_key(&f, "stats")) return H3C_FAILED_UNPACK;
    enum h3c_rc rc = stats_unpack(&result->stats, &f);
    if (rc) return rc;

    if (!expect_key(&f, "tophits")) return H3C_FAILED_UNPACK;
    return tophits_unpack(&result->tophits, &f);
}

void h3c_result_print_targets(struct h3c_result const *r, FILE *file)
{
    tophits_print_targets(&r->tophits, file, r->stats.Z);
}

void h3c_result_print_domains(struct h3c_result const *r, FILE *file)
{
    tophits_print_domains(&r->tophits, file, r->stats.Z, r->stats.domZ);
}

void h3c_result_print_targets_table(struct h3c_result const *r, FILE *file)
{
    tophits_print_targets_table("-", &r->tophits, file, true, r->stats.Z);
}

void h3c_result_print_domains_table(struct h3c_result const *r, FILE *file)
{
    tophits_print_domains_table("-", &r->tophits, file, true, r->stats.Z,
                                r->stats.domZ);
}
