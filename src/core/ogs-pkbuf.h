/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OGS_PKBUF_H
#define OGS_PKBUF_H

#if !defined(OGS_CORE_INSIDE)
#error "Only <ogs-core.h> can be included directly."
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ogs_cluster_s {
    void *buffer;
    unsigned int size;

    unsigned int ref;
} ogs_cluster_t;

typedef struct ogs_pkbuf_pool_s ogs_pkbuf_pool_t;
typedef struct ogs_pkbuf_s {
    ogs_cluster_t *cluster;

    unsigned int len;

    void *head;
    void *tail;
    void *data;
    void *end;
    
    ogs_pkbuf_pool_t *pool;
} ogs_pkbuf_t;

typedef struct ogs_pkbuf_config_s {
    int cluster_128_pool;
    int cluster_256_pool;
    int cluster_512_pool;
    int cluster_1024_pool;
    int cluster_2048_pool;
    int cluster_8192_pool;
    int cluster_big_pool;
} ogs_pkbuf_config_t;

void ogs_pkbuf_init(void);
void ogs_pkbuf_final(void);

void ogs_pkbuf_default_init(ogs_pkbuf_config_t *config);
void ogs_pkbuf_default_create(ogs_pkbuf_config_t *config);
void ogs_pkbuf_default_destroy(void);

ogs_pkbuf_pool_t *ogs_pkbuf_pool_create(ogs_pkbuf_config_t *config);
void ogs_pkbuf_pool_destroy(ogs_pkbuf_pool_t *pool);

ogs_pkbuf_t *ogs_pkbuf_alloc(ogs_pkbuf_pool_t *pool, unsigned int size);
void ogs_pkbuf_free(ogs_pkbuf_t *pkbuf);

ogs_pkbuf_t *ogs_pkbuf_copy(ogs_pkbuf_t *pkbuf);

static ogs_inline int ogs_pkbuf_tailroom(const ogs_pkbuf_t *pkbuf)
{
    return pkbuf->end - pkbuf->tail;
}

static ogs_inline int ogs_pkbuf_headroom(const ogs_pkbuf_t *pkbuf)
{
    return pkbuf->data - pkbuf->head;
}

static ogs_inline void ogs_pkbuf_reserve(ogs_pkbuf_t *pkbuf, int len)
{
    pkbuf->data += len;
    pkbuf->tail += len;
}

static ogs_inline void *ogs_pkbuf_put(ogs_pkbuf_t *pkbuf, unsigned int len)
{
    void *tmp = pkbuf->tail;

    pkbuf->tail += len;
    pkbuf->len += len;

    if (ogs_unlikely(pkbuf->tail > pkbuf->end))
        ogs_assert_if_reached();

    return tmp;
}

static ogs_inline void *ogs_pkbuf_put_data(
        ogs_pkbuf_t *pkbuf, const void *data, unsigned int len)
{
    void *tmp = ogs_pkbuf_put(pkbuf, len);

    memcpy(tmp, data, len);
    return tmp;
}

static ogs_inline void ogs_pkbuf_put_u8(ogs_pkbuf_t *pkbuf, uint8_t val)
{
    *(uint8_t *)ogs_pkbuf_put(pkbuf, 1) = val;
}

static ogs_inline void *ogs_pkbuf_push(ogs_pkbuf_t *pkbuf, unsigned int len)
{
    pkbuf->data -= len;
    pkbuf->len += len;

    if (ogs_unlikely(pkbuf->data < pkbuf->head))
        ogs_assert_if_reached();

    return pkbuf->data;
}

static ogs_inline void *ogs_pkbuf_pull_inline(
        ogs_pkbuf_t *pkbuf, unsigned int len)
{
    pkbuf->len -= len;
    return pkbuf->data += len;
}

static ogs_inline void *ogs_pkbuf_pull(ogs_pkbuf_t *pkbuf, unsigned int len)
{
    return ogs_unlikely(len > pkbuf->len) ?
        NULL : ogs_pkbuf_pull_inline(pkbuf, len);
}

static ogs_inline void ogs_pkbuf_trim(ogs_pkbuf_t *pkbuf, unsigned int len)
{
    if (pkbuf->len > len) {
        pkbuf->tail = pkbuf->data + len;
        pkbuf->len = len;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* OGS_PKBUF_H */
