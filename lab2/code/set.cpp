#include "set.h"
#include "node.h"

int Set::Node::count_nodes = 0;

/*****************************************************
 * Implementation of the member functions             *
 ******************************************************/

/*
 *  Default constructor :create an empty Set
 */
int Set::get_count_nodes() {
    return Set::Node::count_nodes;
}

/*
 *  Default constructor :create an empty Set
 */
Set::Set() : counter{0} {
    // IMPLEMENT before Lab2 HA

    head = new Node();
    tail = new Node();

    head->next = tail;
    tail->prev = head;

}

/*
 *  Conversion constructor: convert val into a singleton {val}
 */
Set::Set(int val) : Set{} {  // create an empty list
    // IMPLEMENT before Lab2 HA

    Node* singleton = new Node(val, tail, head);
    head->next = singleton;
    tail->prev = singleton;
    counter = 1;
}

/*
 * Constructor to create a Set from a sorted vector of unique ints
 * Create a Set with all ints in sorted vector list_of_values
 */
Set::Set(const std::vector<int>& list_of_values) : Set{} {  // create an empty list
    // IMPLEMENT before Lab2 HA

    Node* ptr1 = head;
    for (size_t i = 0; i < list_of_values.size(); i++) {

        Node* ptr = new Node(list_of_values[i], tail, ptr1);
        ptr1->next = ptr;
        tail->prev = ptr;   // uppdateras konstant så att sista varvet, blir tail->prev det sista värdet
        ptr1 = ptr;
        
    }
    counter = list_of_values.size();
}

/*
 * Copy constructor: create a new Set as a copy of Set S
 * \param S Set to copied
 * Function does not modify Set S in any way
 */
Set::Set(const Set& S) : Set{} {  // create an empty list
    // IMPLEMENT before Lab2 HA

    if (S.head->next == S.tail) {
        Set();
        return;
    }

    Node* ptrS = S.head->next;
    Node* ptr1 = head;
    
    while (ptrS->next != nullptr) {

        Node* ptr = new Node(ptrS->value, tail, ptr1);
        ptr1->next = ptr;
        tail->prev = ptr;
        ptr1 = ptr;
        ptrS = ptrS->next;
    }
    counter = S.counter;

}

/*
 * Transform the Set into an empty set
 * Remove all nodes from the list, except the dummy nodes
 */
void Set::make_empty() {
    // IMPLEMENT before Lab2 HA

    Node* ptr1 = head->next;

    while (ptr1->next != nullptr) {

        Node* ptr2 = ptr1;
        ptr1 = ptr1->next;
        delete ptr2;
    }
    head->next = tail;
    tail->prev = head;
    counter = 0;
}

/*
 * Destructor: deallocate all memory (Nodes) allocated for the list
 */
Set::~Set() {
    // IMPLEMENT before Lab2 HA
    this->make_empty();
    delete head;
    delete tail;
    counter = 0;

}

/*
 * Assignment operator: assign new contents to the *this Set, replacing its current content
 * \param S Set to be copied into Set *this
 * Call by valued is used
 */
Set& Set::operator=(Set S) {
    // IMPLEMENT before Lab2 HA

    make_empty();

    Node* ptrS = S.head->next;
    Node* ptr1 = head;

    while (ptrS->next != nullptr) {

        Node* ptr = new Node(ptrS->value, tail, ptr1);
        ptr1->next = ptr;
        tail->prev = ptr;
        ptr1 = ptr;
        ptrS = ptrS->next;
    }
    counter = S.counter;

    return *this;
}

/*
 * Test whether val belongs to the Set
 * Return true if val belongs to the set, otherwise false
 * This function does not modify the Set in any way
 */
bool Set::is_member(int val) const {
    // IMPLEMENT before Lab2 HA

    Node* ptr = head->next;
    while (ptr->next != nullptr) {
        if (ptr->value == val) return true;
        ptr = ptr->next;
    }
    return false;
}

/*
 * Test whether Set *this and S represent the same set
 * Return true, if *this has same elemnts as set S
 * Return false, otherwise
 */
bool Set::operator==(const Set& S) const {
    // IMPLEMENT before Lab2 HA


    if (cardinality() != S.cardinality()) return false;

    Node* ptr1 = head->next;
    Node* ptr2 = S.head->next;
    while (ptr1->next != nullptr) {

        if (ptr1->value != ptr2->value) return false;

        ptr1 = ptr1->next;
        ptr2 = ptr2->next;
    }
    return true;
}

/*
 * Three-way comparison operator: to test whether *this == S, *this < S, *this > S
 * Return std::partial_ordering::equivalent, if *this == S
 * Return std::partial_ordering::less, if *this < S
 * Return std::partial_ordering::greater, if *this > S
 * Return std::partial_ordering::unordered, otherwise
 */
std::partial_ordering Set::operator<=>(const Set& S) const {
    // IMPLEMENT before Lab2 HA
    
    // Om S1 == S2, det kan vi redan checka med föregående funktion.
    if (*this == S) return std::partial_ordering::equivalent;

    // Om de är samma längd, men olika innehåll. Då kan hela S1 inte finnas inuti S2 eller vice versa, för om längderna var samma hade de varit ekvivalenta. 
    // Men vi vet att de inte är ekvivalenta. De har samma längd, men olika värden.
    // Alltså kan inte ett helt set få plats i det andra.
    // Därmed är de inte S1 < S2 eller S1 > S2.
    if (cardinality() == S.cardinality()) return std::partial_ordering::unordered;

    // Om S1 är kortare än S2, kan endast hela S1 få plats i S2.
    if (cardinality() < S.cardinality()) return std::partial_ordering::less;

    // Om inget av ovanstående har returnat något, måste hela S2 finnas inuti S1. 
    // Process of Elimination genom hela denna funktion.
    return std::partial_ordering::greater;
}

/*
 * Modify Set *this such that it becomes the union of *this with Set S
 * Set *this is modified and then returned
 */
Set& Set::operator+=(const Set& S) {
    // IMPLEMENT
    return *this;
}

/*
 * Modify Set *this such that it becomes the intersection of *this with Set S
 * Set *this is modified and then returned
 */
Set& Set::operator*=(const Set& S) {
    // IMPLEMENT
    return *this;
}

/*
 * Modify Set *this such that it becomes the Set difference between Set *this and Set S
 * Set *this is modified and then returned
 */
Set& Set::operator-=(const Set& S) {
    // IMPLEMENT
    return *this;
}


/* ******************************************** *
 * Private Member Functions -- Implementation   *
 * ******************************************** */

/*
 * Insert a new Node storing val after the Node pointed by p
 * \param p pointer to a Node
 * \param val value to be inserted  after position p
 */
void Set::insert_node(Node* p, int val) {
    // IMPLEMENT before Lab2 HA
}

/*
 * Remove the Node pointed by p
 * \param p pointer to a Node
 */
void Set::remove_node(Node* p) {
    // IMPLEMENT before Lab2 HA
}

/*
 * Write Set *this to stream os
 */
void Set::write_to_stream(std::ostream& os) const {
    if (is_empty()) {
        os << "Set is empty!";
    } else {
        Set::Node* ptr{head->next};

        os << "{ ";
        while (ptr != tail) {
            os << ptr->value << " ";
            ptr = ptr->next;
        }
        os << "}";
    }
}
