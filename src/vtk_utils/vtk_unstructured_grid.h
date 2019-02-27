//
// Created by sachetto on 30/10/18.
//

#ifndef MONOALG3D_VTK_UNSTRUCTURED_GRID_H
#define MONOALG3D_VTK_UNSTRUCTURED_GRID_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

struct vtk_unstructured_grid {
    uint32_t num_points;
    uint32_t num_cells;

    //TODO: we handle only meshes with the same number of points per cell. If we need something different we will need to change this
    uint32_t points_per_cell;

    //TODO: we handle only meshes with the same cell_type. If we need something different we will need to change this
    uint8_t cell_type;

    float *values;
    int *cells;
    struct point_3d *points;

};

struct vtk_unstructured_grid *new_vtk_unstructured_grid();

bool new_vtk_unstructured_grid_from_file(struct vtk_unstructured_grid **vtk_grid, char *file, bool clip_with_plain,
                                         float *plain_coordinates, bool clip_with_bounds,
                                         float *bounds, bool read_only_values);

void save_vtk_unstructured_grid_as_vtu(struct vtk_unstructured_grid *vtk_grid, char *filename, bool binary);
void save_vtk_unstructured_grid_as_vtu_compressed(struct vtk_unstructured_grid *vtk_grid, char *filename, int compression_level);
void save_vtk_unstructured_grid_as_legacy_vtk(struct vtk_unstructured_grid *vtk_grid, char *filename, bool binary);
void free_vtk_unstructured_grid(struct vtk_unstructured_grid *vtk_grid);

bool convert_to_files_in_dir_to_vtu(const char *input_dir, const char *output_dir, const char *file_prefix, bool animate_grid, bool compressed, bool plain, bool verbose);

#endif // MONOALG3D_VTK_UNSTRUCTURED_GRID_H
