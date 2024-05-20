#pragma once

#include <iostream>
#include <vector>
#include <cassert>

//#define TEST_PRIORITY_QUEUE

/**
 * A heap based priority queue where the root is the smallest element -- min heap
 */
template <class Comparable>
class PriorityQueue {
public:
    /**
     * Constructor to create a queue with the given capacity
     */
    explicit PriorityQueue(int initCapacity = 100) : orderOK{true} {
        pq.reserve(initCapacity + 1);
        makeEmpty();
        assert(isEmpty());
    }

    /**
     * Constructor to initialize a priority queue based on a given vector V
     * Assumes V[0] is not used
     */
    explicit PriorityQueue(const std::vector<Comparable>& V) : pq{V} {
        heapify();
#ifdef TEST_PRIORITY_QUEUE
        assert(isMinHeap());
#endif
    }

    /**
     * Make the queue empty
     */
    void makeEmpty() {
        pq.clear();
        pq.push_back(Comparable{});
    }

    /**
     * Check is the queue is empty
     * Return true if the queue is empty, false otherwise
     */
    bool isEmpty() const {
        return pq.size() == 1;  // slot zero is not used
    }

    /**
     * Get the size of the queue, i.e. number of elements in the queue
     */
    size_t size() const { return pq.size() - 1; }

    /**
     * Get the smallest element in the queue
     */
    Comparable findMin() {
        assert(isEmpty() == false);
        return pq[1];
    }

    /**
     * Remove and return the smallest element in the queue
     */
    Comparable deleteMin();

    /**
     * Add a new element x to the queue
     */
    void insert(const Comparable& x);

    /**
     * Insert element x in the end of the queue, without preserving the heap property
     */
    void toss(const Comparable& x);

private:
    std::vector<Comparable> pq;  // slot with index 0 not used
    bool orderOK;  // flag to keep internal track of when the heap is ordered / not ordered

    // Auxiliary member functions

    /**
     * Restore the heap-ordering property
     */
    void heapify();

    void percolateDown(size_t i);

    /**
     * Test whether pq is a min heap
     */
    bool isMinHeap() const {
        
        
        // ty heapen egentligen börjar på 1
        // dessutom använder vi 2*i för advancement, vilket inte funkar med i = 0.
        int i = 1;

        // gå igenom hela heapen
        while (i < (pq.size()-1)) {

            // om pq[i] har ett rightChild (då har den också ett leftChild)
            if (2 * i + 1 < pq.size()) {

                // om dess barn är större än den själv
                if (pq[i] < pq[2 * i] && pq[i] < pq[2 * i + 1]) {

                    // gå fram och fortsätt testa
                    i++;
                } 
                // om dess barn istället är mindre -> inget minHeap
                else {
                    return false;
                }
            } 
            // inga barn, gå fram och fortsätt testa
            else {
                i++;
            }
        }

        // om den gick igenom hela, så är det en minHeap
        return true;
    }
};

template <class Comparable>
void PriorityQueue<Comparable>::percolateDown(size_t i) {
    Comparable temp = pq[i];
    auto c = 2 * i;  // left child

    while (c < pq.size()) {
        if (c < pq.size() - 1) {
            if (pq[c + 1] < pq[c])  // largest child?
                c++;
        }
        // percolate down
        if (pq[c] < temp) {
            pq[i] = pq[c];
            i = c;
            c = 2 * i;
        } else {
            break;
        }
    }
    pq[i] = temp;
}

/**
 * Restore the heap property
 */
template <class Comparable>
void PriorityQueue<Comparable>::heapify() {
    assert(pq.size() > 1);  // slot zero is not used

    for (size_t i = (pq.size() - 1) / 2; i >= 1; --i) {
        percolateDown(i);
    }
    orderOK = true;
}

/**
 * Remove and return the smallest element in the queue
 */
template <class Comparable>
Comparable PriorityQueue<Comparable>::deleteMin() {
    assert(!isEmpty());

    if (!orderOK) {
        heapify();
    }

    Comparable x = pq[1];
    Comparable y = pq[pq.size() - 1];

    pq[1] = y;  // set last element in the heap as the new root
    percolateDown(1);
    pq.pop_back();

#ifdef TEST_PRIORITY_QUEUE
    assert(isMinHeap());
#endif
    return x;
}

/**
 * Insert element x on the last slot, without preserving the heap property
 */
template <class Comparable>
void PriorityQueue<Comparable>::toss(const Comparable& x) {
    orderOK = false;
    pq.push_back(x);
}

/**
 * Add a new element x to the queue
 */
template <class Comparable>
void PriorityQueue<Comparable>::insert(const Comparable& x) {
    // TO BE IMPLEMENTED

    // lägg till x som ett löv
    toss(x);

    // börja räknaren vid x:s löv
    int i = pq.size() - 1;

    // gå baklänges till rooten
    while (i > 0) {

        // om den har en parent
        if ((i / 2) > 0) {
            
            // om parent är större än x
            if (pq[i / 2] > x) {
                
                // byt plats på parent och x
                pq[i] = pq[i / 2];
                pq[i / 2] = x;
                i = i / 2; // sen hoppa upp till parenten

            } 
            // om x:s parent är mindre än x, färdig!
            else {
                return;
            }
        } 
        // om den inte har en parent, hoppa upp till parent
        else {
            i = i / 2;
        }
    }


#ifdef TEST_PRIORITY_QUEUE  // do not delete
    assert(isMinHeap());
#endif
}
