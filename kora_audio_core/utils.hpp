#pragma once

#include <iostream>
#include <fstream>

class utils
{
public:
    static void save_array(double *l, double *r, size_t size)
    {
        std::ofstream myfile ("example.txt");
        if (myfile.is_open())
        {
            myfile << "This is a line.\n";
            myfile << "This is another line.\n";
            for(int count = 0; count < size; count ++){
                myfile << l[count] << "\t" << r[count] << std::endl;
            }
            myfile.close();
        }
    }

    static void save_array(float *l, float *r, size_t size)
    {
        std::ofstream myfile ("example.txt");
        if (myfile.is_open())
        {
            myfile << "This is a line.\n";
            myfile << "This is another line.\n";
            for(int count = 0; count < size; count ++){
                myfile << l[count] << "\t" << r[count] << std::endl;
            }
            myfile.close();
        }
    }

    static void save_array2(float *l, float *r, float *ll, float *rr, size_t size)
    {
        std::ofstream myfile ("example.txt");
        if (myfile.is_open())
        {
            for(int count = 0; count < size; count ++)
            {
                myfile << l[count] << "\t" << r[count] << "\t | \t " << ll[count] << "\t" << rr[count] << std::endl;
            }
            myfile.close();
        }
    }
};

