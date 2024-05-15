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

    insert_node(head, val); // ny nod efter head
    tail->prev = head->next;
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

        insert_node(ptr1, list_of_values[i]);
        tail->prev = ptr1->next;   // uppdateras konstant s� att sista varvet, blir tail->prev det sista v�rdet
        ptr1 = ptr1->next;
        
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

        insert_node(ptr1, ptrS->value);
        tail->prev = ptr1->next;
        ptr1 = ptr1->next;
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

    Node* ptr = head;
    while (ptr->next != tail) {
        remove_node(ptr->next);
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
    // T(n) = O(n1), eftersom den g�r stegvis genom S1 f�r att t�mma den.

    Node* ptrS = S.head->next;
    Node* ptr1 = head;
    // T(n) = O(2), tv� konstanta steg

    while (ptrS->next != nullptr) {

        insert_node(ptr1, ptrS->value);
        tail->prev = ptr1->next;
        ptr1 = ptr1->next;
        ptrS = ptrS->next;
    }
    // T(n) = O(n2), f�r att nu g�r man igenom hela S2 stegvis f�r att bygga upp S1 fr�n grunden.

    counter = S.counter;
    // T(n) = O(1), konstant

    return *this;
}

//          Resultatet blir T(n) = n1 + n2 + 3, vilket ger O(n), linj�rt.


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
    
    // Om S1 == S2, det kan vi redan checka med f�reg�ende funktion.
    if (*this == S) return std::partial_ordering::equivalent;

    // Om de �r samma l�ngd, men olika inneh�ll. D� kan hela S1 inte finnas inuti S2 eller vice versa, f�r om l�ngderna var samma hade de varit ekvivalenta. 
    // Men vi vet att de inte �r ekvivalenta. De har samma l�ngd, men olika v�rden.
    // Allts� kan inte ett helt set f� plats i det andra.
    // D�rmed �r de inte S1 < S2 eller S1 > S2.
    if (cardinality() == S.cardinality()) return std::partial_ordering::unordered;

    // Om S1 �r kortare �n S2, kan endast hela S1 f� plats i S2.
    if (cardinality() < S.cardinality()) {
        Node* ptr1 = head->next;
        Node* ptr2 = S.head->next;


        while (ptr1->next != nullptr) { //ptr1 blir prioriterad d� denne �r kortast

            if (ptr1->value < ptr2->value) {
                return std::partial_ordering::unordered;
            }
            if (ptr1->value > ptr2->value) {
                ptr2 = ptr2->next;
            }
            if (ptr1->value == ptr2->value) {
                ptr1 = ptr1->next;
                ptr2 = ptr2->next;
            }
        }
        return std::partial_ordering::less; //Om hela ptr1 gick att ta sig igenom M�STE alla element i S1 finnas i S2
    }
    // Om inget av ovanst�ende har returnat n�got, m�ste hela S2 finnas inuti S1. 
    // Process of Elimination genom hela denna funktion.
    if (cardinality() > S.cardinality()) {
        Node* ptr1 = head->next;
        Node* ptr2 = S.head->next;


        while (ptr2->next != nullptr) { //ptr2 blir prioriterad d� denne �r kortast

            if (ptr1->value > ptr2->value) {
                return std::partial_ordering::unordered;
            }
            if (ptr1->value < ptr2->value) {
                ptr2 = ptr2->next;
            }
            if (ptr1->value == ptr2->value) {
                ptr1 = ptr1->next;
                ptr2 = ptr2->next;
            }
        }
        return std::partial_ordering::greater; //Om hela ptr2 gick att ta sig igenom M�STE alla element i S1 finnas i S2
    }
    return std::partial_ordering::unordered;
}

/*
 * Modify Set *this such that it becomes the union of *this with Set S
 * Set *this is modified and then returned
 */
Set& Set::operator+=(const Set& S) {
    // IMPLEMENT

    Node* ptr1 = head;
    Node* ptr2 = S.head;

    while (ptr1->next != nullptr && ptr2->next != nullptr) {

        // Om ptr1:s v�rde �r mindre �n ptr2:s v�rde, g� fram med ptr1.
        if (ptr1->value < ptr2->value) {

            // Specialfall, om ptr1 redan har n�tt tail, l�gg till ptr2:s v�rde efter, d� detta �r st�rre.
            if (ptr1->next == tail) {
                insert_node(ptr1, ptr2->value);
            }
            ptr1 = ptr1->next;
        }
        // Om ptr1:s v�rde �r st�rre �n ptr2:s v�rde, l�gg till ptr2:s v�rde f�re ptr1 och sedan g� fram med ptr2.
        else if (ptr1->value > ptr2->value) {

            insert_node(ptr1->prev, ptr2->value);
            ptr2 = ptr2->next;
        }
        // Om v�rdena �r lika, g� fram med b�da.
        else {
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        }
    }

    // Denna kommer ocks� g� igenom b�da setten, endast tv� g�nger, linj�rt f�r vardera.
    // Detta ger igen T(n) = n1 + n2 + C.
    // D� S1 �r en singleton (k), blir n1 = 1 och blir allts� �nnu en konstant.
    // D�rmed blir      T(n) = n2 + C = O(n) 



    return *this;
}

/*
 * Modify Set *this such that it becomes the intersection of *this with Set S
 * Set *this is modified and then returned
 */
Set& Set::operator*=(const Set& S) {
    // IMPLEMENT


    Node* ptr1 = head;
    Node* ptr2 = S.head;

    while (ptr1->next != nullptr && ptr1->next->next != nullptr && ptr2->next != nullptr && ptr2->next->next != nullptr) {

        // Om ptr1:s v�rde �r mindre �n ptr2:s v�rde, ta bort ptr1.
        if (ptr1->next->value < ptr2->next->value) {

            remove_node(ptr1->next);
        }
        // Om ptr1:s v�rde �r st�rre �n ptr2:s v�rde, g� fram med ptr2.
        else if (ptr1->next->value > ptr2->next->value) {

            // Specialfall, om ptr1 och ptr2 n�dde sista, kan man inte j�mf�ra n�stkommande v�rden, s� ta bort ptr1 �nd�.
            if (ptr2->next->next == S.tail) {
                remove_node(ptr1->next);
            }

            ptr2 = ptr2->next;
        }
        // Om v�rdena �r lika, g� fram med b�da.
        else {
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        }
    }

    // Denna anv�nder tv� pointers, men b�da setten blir bara genoms�kta 1 g�ng var.
    // P� s� s�tt blir det n1 + n2, d� varje element kommer genoms�kas �nd�.
    // Inom varje varv sker endast konstanta operationer (C).
    //      T(n) = n1 + n2 + C = O(n)

    return *this;
}

/*
 * Modify Set *this such that it becomes the Set difference between Set *this and Set S
 * Set *this is modified and then returned
 */
Set& Set::operator-=(const Set& S) {
    // IMPLEMENT


    Node* ptr1 = head;
    Node* ptr2 = S.head;

    while (ptr1->next != nullptr && ptr1->next->next != nullptr && ptr2->next != nullptr && ptr2->next->next != nullptr) {

        // Om ptr1:s v�rde �r mindre �n ptr2:s v�rde, beh�ll och g� fram med ptr1.
        if (ptr1->next->value < ptr2->next->value) {

            ptr1 = ptr1->next;
        }
        // Om ptr1:s v�rde �r st�rre �n ptr2:s v�rde, beh�ll och g� fram med ptr2.
        else if (ptr1->next->value > ptr2->next->value) {

            ptr2 = ptr2->next;
        }
        // Om v�rdena �r lika, ta bort ptr1.
        else {
            remove_node(ptr1->next);
        }
    }


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

    Node* newNode = new Node(val, p->next, p);
    p->next = newNode;
    newNode->next->prev = newNode;

    counter++;
    
}

/*
 * Remove the Node pointed by p
 * \param p pointer to a Node
 */
void Set::remove_node(Node* p) {
    // IMPLEMENT before Lab2 HA

    Node* tmp = p;

    p->prev->next = p->next;
    p = p->next;
    p->prev = tmp->prev;

    counter--;
    
    delete tmp;
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
