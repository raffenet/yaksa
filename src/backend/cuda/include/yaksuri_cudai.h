/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#ifndef YAKSURI_CUDAI_H_INCLUDED
#define YAKSURI_CUDAI_H_INCLUDED

#include "yaksi.h"
#include <stdint.h>
#include <pthread.h>
#include <cuda_runtime_api.h>

#define YAKSURI_CUDAI_THREAD_BLOCK_SIZE  (256)

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define YAKSURI_CUDAI_CUDA_ERR_CHECK(cerr)                              \
    do {                                                                \
        if (cerr != cudaSuccess) {                                      \
            fprintf(stderr, "CUDA Error (%s:%s,%d): %s\n", __func__, __FILE__, __LINE__, cudaGetErrorString(cerr)); \
        }                                                               \
    } while (0)

#define YAKSURI_CUDAI_CUDA_ERR_CHKANDJUMP(cerr, rc, fn_fail)            \
    do {                                                                \
        if (cerr != cudaSuccess) {                                      \
            fprintf(stderr, "CUDA Error (%s:%s,%d): %s\n", __func__, __FILE__, __LINE__, cudaGetErrorString(cerr)); \
            rc = YAKSA_ERR__INTERNAL;                                   \
            goto fn_fail;                                               \
        }                                                               \
    } while (0)

typedef struct {
    cudaStream_t stream;
} yaksuri_cudai_global_s;
extern yaksuri_cudai_global_s yaksuri_cudai_global;

typedef struct yaksuri_cudai_md_s {
    union {
        struct {
            int count;
            intptr_t stride;
            struct yaksuri_cudai_md_s *child;
        } contig;
        struct {
            struct yaksuri_cudai_md_s *child;
        } dup;
        struct {
            struct yaksuri_cudai_md_s *child;
        } resized;
        struct {
            int count;
            int blocklength;
            intptr_t stride;
            struct yaksuri_cudai_md_s *child;
        } hvector;
        struct {
            int count;
            int blocklength;
            intptr_t *array_of_displs;
            struct yaksuri_cudai_md_s *child;
        } blkhindx;
        struct {
            int count;
            int *array_of_blocklengths;
            intptr_t *array_of_displs;
            struct yaksuri_cudai_md_s *child;
        } hindexed;
    } u;

    uintptr_t extent;
    uintptr_t num_elements;
} yaksuri_cudai_md_s;

typedef struct yaksuri_cudai_type_s {
    yaksuri_cudai_md_s *md;
    pthread_mutex_t mdmutex;
    uintptr_t num_elements;
} yaksuri_cudai_type_s;

int yaksuri_cudai_finalize_hook(void);
int yaksuri_cudai_type_create_hook(yaksi_type_s * type, yaksur_gpudev_pup_fn * pack,
                                   yaksur_gpudev_pup_fn * unpack);
int yaksuri_cudai_type_free_hook(yaksi_type_s * type);

int yaksuri_cudai_event_create(void **event);
int yaksuri_cudai_event_destroy(void *event);
int yaksuri_cudai_event_query(void *event, int *completed);
int yaksuri_cudai_event_synchronize(void *event);

int yaksuri_cudai_get_ptr_attr(const void *buf, yaksur_ptr_attr_s * ptrattr);

int yaksuri_cudai_md_alloc(yaksi_type_s * type);
int yaksuri_cudai_populate_pupfns(yaksi_type_s * type, yaksur_gpudev_pup_fn * pack,
                                  yaksur_gpudev_pup_fn * unpack);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* YAKSURI_CUDAI_H_INCLUDED */
