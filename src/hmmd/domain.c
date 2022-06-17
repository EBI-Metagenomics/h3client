#include "hmmd/domain.h"
#include "c_toolbelt/c_toolbelt.h"
#include "h3client/rc.h"
#include "hmmd/alidisplay.h"
#include "hmmd/utils.h"
#include <stdlib.h>

enum h3c_rc hmmd_domain_unpack(struct hmmd_domain *dom, size_t *read_size,
                               unsigned char const *data)
{
    enum h3c_rc rc = H3C_OK;
    *read_size = 0;
    unsigned char const *ptr = data;

    size_t obj_size = eat32(&ptr);
    dom->ienv = eat64(&ptr);
    dom->jenv = eat64(&ptr);
    dom->iali = eat64(&ptr);
    dom->jali = eat64(&ptr);
    dom->iorf = eat64(&ptr);
    dom->jorf = eat64(&ptr);
    to_float(&dom->envsc, eat32(&ptr));
    to_float(&dom->domcorrection, eat32(&ptr));
    to_float(&dom->dombias, eat32(&ptr));
    to_float(&dom->oasc, eat32(&ptr));
    to_float(&dom->bitscore, eat32(&ptr));
    to_double(&dom->lnP, eat32(&ptr));
    dom->is_reported = eat32(&ptr);
    dom->is_included = eat32(&ptr);

    unsigned length = eat32(&ptr);

    if (length > 0)
    {
        if (dom->scores_per_pos != 0)
        {
            free(dom->scores_per_pos);
        }
        dom->scores_per_pos =
            ctb_realloc(dom->scores_per_pos, length * sizeof(float));

        for (unsigned i = 0; i < length; i++)
        {
            to_float(&dom->scores_per_pos[i], eat32(&ptr));
        }
    }

    if (ptr != obj_size + data)
    {
        rc = H3C_INVALID_PACK;
        goto cleanup;
    }

    // TODO: finish this
    // finally, the enclosed P7_ALIDISPLAY object
    if (dom->ad == 0)
    {
        // domain->ad = p7_alidisplay_Create_empty();
    }
    size_t size = 0;
    if ((rc = hmmd_alidisplay_unpack(dom->ad, &size, ptr))) goto cleanup;
    ptr += size;

    *read_size = (size_t)(ptr - data);
    return H3C_OK;

cleanup:
    return rc;
}
