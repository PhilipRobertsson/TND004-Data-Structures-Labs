// lab1.cpp : stable partition
// Iterative and divide-and-conquer

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <format>
#include <functional>
#include <cassert>


/****************************************
 * Declarations                          *
 *****************************************/

// generic class to write an item to a stream
template <typename T>
class Formatter {
public:
    Formatter(std::ostream& os, int width, int per_line)
        : os_{os}, per_line_{per_line}, width_{width} {
    }

    void operator()(const T& t) {
        os_ << std::format("{:{}}", t, width_);
        if (++outputted_ % per_line_ == 0)
            os_ << "\n";
    }

private:
    std::ostream& os_;    // output stream
    const int per_line_;  // number of columns per line
    const int width_;     // column width
    int outputted_{0};    // counter of number of items written to os_
};

/* ************************ */

namespace TND004 {
// Iterative algorithm
void stable_partition_iterative(std::vector<int>& V, std::function<bool(int)> p);

// Auxiliary function that performs the stable partition recursively
std::vector<int>::iterator stable_partition(std::vector<int>::iterator first,
                                            std::vector<int>::iterator last,
                                            std::function<bool(int)> p);

// Divide-and-conquer algorithm
void stable_partition(std::vector<int>& V, std::function<bool(int)> p) {
    TND004::stable_partition(std::begin(V), std::end(V), p);  // call auxiliary function
}
}  // namespace TND004

void execute(std::vector<int>& V, const std::vector<int>& res);

bool even(int i);

/****************************************
 * Main:test code                        *
 *****************************************/

int main() {
    /*****************************************************
     * TEST PHASE 1                                       *
     ******************************************************/
    {
        std::cout << "TEST PHASE 1\n\n";

        std::vector<int> seq{1, 2};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>{std::cout, " "});

        execute(seq, std::vector<int>{2, 1});

        std::cout << "\nEmpty sequence: ";
        std::vector<int> empty;

        execute(empty, std::vector<int>{});
    }

    /*****************************************************
     * TEST PHASE 2                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 2\n\n";

        std::vector<int> seq{2};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>{std::cout, " "});

        execute(seq, std::vector<int>{2});
    }

    /*****************************************************
     * TEST PHASE 3                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 3\n\n";

        std::vector<int> seq{3};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>{std::cout, " "});

        execute(seq, std::vector<int>{3});
    }

    /*****************************************************
     * TEST PHASE 4                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 4\n\n";

        std::vector<int> seq{3, 3};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>(std::cout, " "));

        execute(seq, std::vector<int>{3, 3});
    }

    /*****************************************************
     * TEST PHASE 5                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 5\n\n";

        std::vector<int> seq{1, 2, 3, 4, 5, 6, 7, 8, 9};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>(std::cout, " "));

        execute(seq, std::vector<int>{2, 4, 6, 8, 1, 3, 5, 7, 9});
    }

    /*****************************************************
     * TEST PHASE 6                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 6: test with long sequence loaded from a file\n\n";

        std::ifstream file("../code/test_data.txt"); // if mac then change this path

        if (!file) {
            std::cout << "Could not open test_data.txt!!\n";
            return 0;
        }

        // read the input sequence from file
        std::vector<int> seq{std::istream_iterator<int>{file}, std::istream_iterator<int>()};
        file.close();

        std::cout << "\nNumber of items in the sequence: " << std::ssize(seq) << '\n';

        /*std::cout << "Sequence:\n";
        std::for_each(std::begin(seq), std::end(seq), Formatter<int>(std::cout, 8, 5));*/

        // read the result sequence from file
        file.open("../code/test_result.txt");  // if mac then change this path

        if (!file) {
            std::cout << "Could not open test_result.txt!!\n";
            return 0;
        }

        std::vector<int> res{std::istream_iterator<int>{file}, std::istream_iterator<int>()};

        std::cout << "\nNumber of items in the result sequence: " << std::ssize(res);

        // display expected result sequence
        // std::for_each(std::begin(res), std::end(res), Formatter<int>(std::cout, 8, 5));

        assert(std::ssize(seq) == std::ssize(res));

        execute(seq, res);
    }
}

/****************************************
 * Functions definitions                 *
 *****************************************/

bool even(int i) {
    return i % 2 == 0;
}

// Used for testing
void execute(std::vector<int>& V, const std::vector<int>& res) {
    std::vector<int> copy_{V};

    std::cout << "\n\nIterative stable partition\n";
    TND004::stable_partition_iterative(V, even);
    assert(V == res);  // compare with the expected result

    // Uncomment for exercise 2
    std::cout << "Divide-and-conquer stable partition\n";
    TND004::stable_partition(copy_, even);
    assert(copy_ == res);  // compare with the expected result
}

// Iterative algorithm, Linear growth of estimated time according to O(n) with size given as S(n) = O(n)
void TND004::stable_partition_iterative(std::vector<int>& V, std::function<bool(int)> p) {
    std::vector<int> trueVal; //S(1)
    std::vector<int> falseVal; //S(1)
    for (size_t i = 0; i < V.size(); i++) { //S(n), T(n) => O(n)
        if (p(V[i])) { trueVal.push_back(V[i]); } // T(1) according to https://cplusplus.com/reference/vector/vector/push_back/
        else           { falseVal.push_back(V[i]); } // T(1) => O(1)
    }
    falseVal.insert(falseVal.begin(), trueVal.begin(), trueVal.end()); //T(n) accoring to https://cplusplus.com/reference/vector/vector/insert/ =>O(n)
    V = falseVal; // T(n) => O(n)
}

// Auxiliary function that performs the stable partition recursively
// Divide-and-conquer algorithm: stable-partition the sub-sequence starting at first and ending
// at last-1. If there are items with property p then return an iterator to the end of the block
// containing the items with property p. If there are no items with property p then return first.

//According to the master theorem when we have a = 3, b=2, c=2 and
// f(n) = (6+2) => O(n) => k = 1.
// Therefore, b = c^k => 2 = 2^1.
// This gives T(n) = O(n*log n)

// In a similar manner O(n) for S(n) is given by the master theroem with a = 0, b = 2, c = 2
// f(n) = 3 => O(1) => k = 0.
// Therefore, b > c^k => 2 > 2^0.
// This gives S(n) = O(n^(logc(b)) => O(n^(log2(2)) => O(n).
std::vector<int>::iterator TND004::stable_partition(std::vector<int>::iterator first,
                                                    std::vector<int>::iterator last,
                                                    std::function<bool(int)> p) {
    // Comparison T(1) =>O(1)
    if (first == last) return first; // If first and last are pointing to the same item the length of the vector is 0, base case

    // Function call with constant complexity given that the iterators are random access T(1)=>O(1) and S(1)
    // according to https://cplusplus.com/reference/iterator/distance/?kw=distance
    int vecLength = std::distance(first, last); // Calculate disance between first and last item

    // Comparrison T(1) => O(1)
    if (vecLength == 1) { // If only two items are present

        //Function call T(1) => O(1)
        if (p(*first)) { // First item is true, thus this value can be skipped
            return last; // Move pointer to the next item
        }
        else { // First item is false, keep this value for later
            return first; // Keep pointer on the current item
        }
    }

    // Consant assignment T(3) = O(1), and S(1)
    std::vector<int>::iterator midIt = first + vecLength / 2; // Middle item in the vector

    //std::cout<<"Mid value: " << *midIt << "\n";

    // Split vector into left side, it1 in the range [first,midIt], and the right side, it2 int the range [midIt,last]
    std::vector<int>::iterator it1 = TND004::stable_partition(first, midIt, p); 
    std::vector<int>::iterator it3 = TND004::stable_partition(midIt,last, p);

    // Rotate both sides according to the appendix to ensure that the first half of the vector only includes true items 
    // while the last half only includes false items
    // Complexity is linear according to https://cplusplus.com/reference/algorithm/rotate/ and thus T(n) = O(n) and S(1)
    std::vector<int>::iterator it4 = std::rotate(it1, midIt, it3);

    return it4;
}


// According to a graph from lecture 2 we can see that the recursive algorithm works best for smaller values of n compared to the iterative algoritm given that:
// For small n, O(n*log n) < O(n).
// For large n, O(n) > O(n*log n).
// Both algorithms have the same S(n) = O(n) and there is therfore no diffrence between the required sizes depending on the value n.
