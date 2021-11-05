#include <array>
#include <cstdint>
#include <iostream>
#include <random>
#include <cassert>

#include <CL/sycl.hpp>

using namespace std;

class reduction_kernel;
namespace sycl = cl::sycl;

int main()
{
    size_t group_size = 32, part_size = group_size * 2;
    const int N = 8;
    array<int32_t, N> arr = {1, 2, 3, 4, 5, 6, 7, 8};
    for (auto &el : arr)
    {
        cout << el << " ";
    }
    cout << endl;

    sycl::buffer<int32_t, 1> buf(arr.data(), sycl::range<1>(arr.size()));

    sycl::device device = sycl::default_selector{}.select_device();

    sycl::queue queue(device, [](sycl::exception_list el)
                      {
                          for (auto ex : el)
                          {
                              rethrow_exception(ex);
                          }
                      });

    // <<Reduction loop>>
    cout << "Début de réduction" << endl;
    auto len = arr.size();
    int *layer = sycl::malloc_shared<int>(4, queue);
    *layer = 0;
    while (len != 1)
    {
        // division rounding up
        auto n_wgroups = (len + part_size - 1) / part_size;

        queue.submit([&](sycl::handler &cgh)
                     {
                         sycl::accessor<int32_t, 1, sycl::access::mode::read_write, sycl::access::target::local>
                             local_mem(sycl::range<1>(group_size), cgh);

                         sycl::stream out(1024, 256, cgh);

                         auto global_mem = buf.get_access<sycl::access::mode::read_write>(cgh);
                         cgh.parallel_for<class reduction_kernel>(
                             sycl::nd_range<1>(n_wgroups * group_size, group_size),
                             [=](sycl::nd_item<1> item)
                             {
                                 size_t local_id = item.get_local_linear_id();
                                 size_t global_id = item.get_global_linear_id();
                                 local_mem[local_id] = 0;

                                 if ((2 * global_id) < len)
                                 {
                                     local_mem[local_id] = global_mem[2 * global_id] + global_mem[2 * global_id + 1];

                                     // printing the operation
                                     if (global_mem[2 * global_id] != 0 && global_mem[2 * global_id + 1] != 0)
                                     {
                                         /*sycl::string_class tab = "";
                            for (int i = 0; i < *layer; i++) {
                                tab = tab + "  ";
                            }*/
                                         out << "Niveau " << *layer << " : ";
                                         out << global_mem[2 * global_id] << " + " << global_mem[2 * global_id + 1];
                                         out << " = " << local_mem[local_id] << sycl::endl;

                                         (*layer)++;
                                     }
                                 }
                                 item.barrier(sycl::access::fence_space::local_space);

                                 for (size_t stride = 1; stride < group_size; stride *= 2)
                                 {
                                     auto idx = 2 * stride * local_id;
                                     if (idx < group_size)
                                     {

                                         auto sum = local_mem[idx] + local_mem[idx + stride];

                                         // printing operation
                                         if (local_mem[idx] != 0 && local_mem[idx + stride] != 0 && sum != 0)
                                         {
                                             out << "Niveau " << *layer << " : ";
                                             out << local_mem[idx] << " + " << local_mem[idx + stride];
                                             out << " = " << sum << sycl::endl;
                                             (*layer)++;
                                         }
                                         local_mem[idx] = sum;
                                     }

                                     item.barrier(sycl::access::fence_space::local_space);
                                 }

                                 if (local_id == 0)
                                 {
                                     global_mem[item.get_group_linear_id()] = local_mem[0];
                                 }
                             });
                     });
        queue.wait_and_throw();
        len = n_wgroups;
    }

    auto acc = buf.get_access<sycl::access::mode::read>();
    cout << "Somme des éléments: " << acc[0] << endl;

    return 0;
}