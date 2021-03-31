//
// Created by Alexander Rossiter on 30/03/2021.
//

#include "GridWriter.h"

void GridWriter::write_grid(std::string& fname) {
    std::ofstream file;
    file.open(fname);

    for (Block& b : g.get_blocks()) {
        file << "Block " << b.id << std::endl;
        file << b.ni << " " << b.nj << " " << b.nk << std::endl;
        for (int i = 1; i < b.ni+1; i++) {
            for (int j = 1; j < b.nj+1; j++) {
                for (int k = 1; k < b.nk+1; k++) {
                    file << b.x[i][j][k] << " " << b.y[i][j][k] << " " << b.z[i][j][k] << " "
                    << b.volume[i][j][k] << " " << b.geom[i][j][k].A[0] << " " << b.geom[i][j][k].A[1]
                    << " " << b.geom[i][j][k].A[2] << " " << b.isWall[i][j][k] << " " << b.ro[i][j][k]
                    << " " << b.rovx[i][j][k] << " " << b.rovy[i][j][k] << " " << b.rovz[i][j][k] << " "
                    << b.roe[i][j][k] << std::endl;
                }
            }
        }
    }

}

void GridWriter::write_solution(std::string& fname) {

}
