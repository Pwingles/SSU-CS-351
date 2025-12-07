
#include <algorithm>
#include <barrier>
#include <cmath>
#include <cstdlib>
#include <format>
#include <iostream>
#include <numeric>
#include <random>
#include <thread>
#include <unistd.h>
#include <vector>

#include "Shapes.h"

/////////////////////////////////////////////////////////////////////////////
//
// --- sdf() ---
//
// A signed-distance function, which when provided with a 3D point (in this
//   case), it will return a boolean value indicating if the value is inside
//   the region of interest.  In this case, passed parameter (p) is expected
//   to be a point within the unit cube (i.e., (x, y, z) are all values 
//   in [0.0, 1.0]).
//
// p is transformed into the sphere's coordinate system (by subtracting off
//   the sphere's center), and then p's distance from the origin is computed
//   and compared to the sphere's radius.  As we're looking to compute the
//   volume of the cube with the sphere removed, we're interested to know if
//   the point is inside or outside of the sphere.
//

bool sdf(vec3 p) {
    // Create a sphere centered inside of our cube
    static const Sphere sphere(vec3(0.5), 0.5);
    
    // Determine if the point is outside the sphere.  It's guaranteed
    //   to be inside the unit cube because of how the points are
    //   generated.
    p -= sphere.center;
    return p.length() > sphere.radius;
}

/////////////////////////////////////////////////////////////////////////////
//
// --- main ---
//

int main(int argc, char* argv[]) {
    //
    // Specify the program's default options:
    //   * numSamples - the default number of points to test with
    //   * partitions - how many pieces the generator’s output interval
    //       is split into
    //   * numThreads - the default number of threads spawnedd
    //
    size_t numSamples = 2'000'000;
    size_t partitions = 1'000'000;
    size_t numThreads = 4;

    //-----------------------------------------------------------------------
    //
    // Process command-line options.  Accepted options are specified in the
    //   character string "options".  All options are single characters
    //   (as compared to full words like some programs).  If a character
    //   is followed by a color (:), the option expects an additional
    //   parameter.
    //
    // Options are processed using a simple library function getopt()
    //
    int option;
    const char* options = "hp:n:t:";
    while ((option = getopt(argc, argv, options)) != -1) {
        switch (option) {
            case 'h': {
                const char* help =
                    "Usage: %s -[hpnt]\n"
                    "    -h           show help message\n"
                    "    -p <value>   paritions for uniform number generator (default :%u)\n"
                    "    -n <value>   total number of sample points (default :%u)\n"
                    "    -t <value>   use <values> number of threads (default: %u)\n";

                    fprintf(stderr, help, argv[0], partitions, numSamples, numThreads);
                    exit(EXIT_SUCCESS);
            } break;

            case 'p':
                partitions = std::stol(optarg);
                break;

            case 'n':
                numSamples = std::stol(optarg);
                break;

            case 't':
                numThreads = std::stol(optarg);
                break;
        }
    }

    //-----------------------------------------------------------------------
    //
    // A collection of variables to make threading the application simpler.
    //
    //   * threads - is merely an array of std::jthreads to store the created
    //       threads, similar to what was demonstrated in class
    //   * insidePoints - provides a per-thread variable allowing you to 
    //       accumulate the values computed in a thread independent of other
    //       threads
    //   * barrier - provides a synchronization barrier to prevent threads
    //       exiting before their peers
    //
    std::vector<std::jthread>  threads(numThreads);
    std::vector<size_t>        insidePoints(numThreads);
    std::barrier               barrier(numThreads);

    // Computation of how much work a thread should do. (yes, this doesn't
    //   divide evenly in most cases, but we'll lose at most numThreads
    //   samples, which isn't a big deal if the numSamples is sufficiently
    //   large)
    size_t chunkSize = numSamples / numThreads;

    //-----------------------------------------------------------------------
    //
    // The computational kernel, where you should enter your thread
    //   implementation.  Much of the thread framework is provided below,
    //   and you mostly need to:
    //     * set up the lambda's closure arguments
    //     * implement the kernel's core iteration loop
    //     * collect the results of testing point p's position using the
    //         sdf() function.  (Hint: you can simply tally up the results
    //         returned by sdf() to count how many samples are in the
    //         region of interest).  Something like 
    //
    //           tallyVariable += sdf(p);
    //
    //         will do nicely.
    //
    //   (really, you'll need to add four lines to this, one of which is a
    //      closing brace :-)   
    //
    for (size_t id = 0; id < threads.size(); ++id) {
        threads[id] = std::jthread{ [&, id, chunkSize, partitions]() {

            // C++ 11's random number generation system.  These functions
            //   will generate uniformly distributed unsigned integers in
            //   the range [0, partitions].  The functions are used in the
            //   helper function rand() (implemented as a lambda)
            std::random_device device;
            std::mt19937 generator(device());
            std::uniform_int_distribution<unsigned int> uniform(0.0, partitions);

            // Define a helper function to generate random floating-point
            //   values in the range [0.0, 1.0]
            auto rand = [&,partitions]() {
                return static_cast<double>(uniform(generator)) / partitions;
            };
            
            // Tally the points inside the region of interest
            size_t localCount = 0;
            for (size_t i = 0; i < chunkSize; ++i) {
                // Generate a random point inside the unit cube
                vec3 p(rand(), rand(), rand());
                // Check if the point is in the region (cube minus sphere)
                localCount += sdf(p);
            }
            
            // Store the local count in the per-thread array
            insidePoints[id] = localCount;

            // Wait on the barrier for synchronization
            barrier.arrive_and_wait();
        }};
    }

    // Add in the last necessary parts for our threaded programs.  These
    //   may include summing up the individual threads' computations, or
    //   having the main thread wait on a thread to keep it from exiting
    //
    // (Look in threaded.cpp for hints)
    threads.back().join();

    // Sum up the points from all threads
    size_t totalInsidePoints = std::accumulate(std::begin(insidePoints), std::end(insidePoints), size_t(0));

    // Output the volume estimate (ratio of inside points to total samples)
    // Since we're working with a unit cube, the volume is just the ratio
    std::cout << static_cast<double>(totalInsidePoints) / numSamples << "\n";
}

