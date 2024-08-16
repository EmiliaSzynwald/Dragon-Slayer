/*     CSC 16 Lab Part II    */

#include<iostream>
#include<cstring>  /* for memcpy */
#include<memory>   /* requires -std=c++14 */
//#include<concepts> /* requires -std=c++20 */
using namespace std;

/*  For this assignment, you are to complete an implementation of a "circular
queue" data structure.  Unlike a stack, where you can only push/pop at one
end, in a `circqueue` you can insert/remove values at both ends.  This is
done by allowing the queue to wrap around when it reaches the left or right
of underlying array.  Besides the array, which should be a unique_ptr<T[]>
(under template<typename T>), we need to maintain an index `front` indicating
the position of the first value, as well as integers size and capacity 
indicating the number of values currently in Q and Q's capacity (length of
array).  The capacity should be resizable, like in the varstack program.

To wrap around, we apply the % operator on the capacity (length of array).
To move index i to the right, possibly wrapping around, do (i+1)%capacity.
To move it to the left, possibly wrapping around the left end, do
(i+capacity-1)%capacity.  (Don't do (i-1)%capacity because i could be 0).
Because of the wrapping-around, the queue could look like this:

   0123456789
  |xxx.....xx|

  The x's indicate occupied slots and .'s represent the free slots.
  In this situation, front = 8, size = 5 and capacity = 10.

We have to distinguish between two notions of "index": the *logical*
index and the *real* index.  The logical index of the first item
should always be 0, and the logical index of the last item should be
size-1, just like in a regular array.  But the real index
corresponding to logical index 0 is 8, and the real index of the last
item is 2.  To compute the real index from logical index i, we can
apply the formula

           (front + i) % capacity

  For example, the real index of the last item in the queue is always
  (front + size -1)% capacity.

Consult the varstack sample program: many elements will be similar.
However, there's one very important difference: the varstack assumes
that the type of values T can be copied, whereas for the circqueue you
can only assume that they can be MOVED.

I've implemented part of the structure for you.  Run the included main
to test your program.
*/

typedef unsigned int uint;  // just to save some typing

template<typename T> //requires std::movable<T> && std::destructible<T>
class circqueue {
protected:   // protected means item is visible in this class and subclasses
  unique_ptr<T[]> Q;
  uint front;         // index of first value in Q (invalid if size==0)
  uint size;          // number of values currently in Q
  uint capacity;      // length of Q array (resizable).
  //(front+size-1)%capacity gives you rear

  // the following calculates the real index in Q of logical index i
  inline uint index(uint i) {return (front+i)%capacity;}
  // calculates real index of cell to the "left" of real index i
  inline uint left_index(uint i) {return (i+capacity-1)%capacity;}
  // calculates real index of cell to the "right" of real index i  
  inline uint right_index(uint i) {return (i+1)%capacity;}
public:
  circqueue(uint cap=16): front{0}, size{0} {
    if (cap<1) cap = 1;
    capacity = cap;
    Q = make_unique<T[]>(cap);    
  }//constructor

  uint len() { return size; }
  uint current_capacity() { return capacity; }

  T pop_front() { // reMOVEs and returns the value at front of queue
    if (size==0) throw "nothing to pop";
    T answer = move(Q[front]);
    front = (front+1)%capacity;  // same as right_index(front);
    return answer;
  }
  
  // other methods you need to implement:

  void resize(float factor = 2.0){  //change capacity of Q to capacity*factor
    uint newcap = (uint) (capacity * factor + 0.5);
    if ((size) > newcap) return;
    unique_ptr<T[]> newQ = make_unique<T[]>(newcap);
    for(int i=0;i<capacity;i++){newQ[i]=move(Q[i]);}
    uint oldcap = capacity;
    capacity = newcap;
    Q = move(newQ);

    if (front > ((front+size-1)%oldcap)){
      int difference_in_capacity = capacity - oldcap;
      for (int i = 0; i < oldcap-front;i++){
        std::swap(Q[front+i],Q[capacity-(oldcap-front)+i]);
      }
      front = front + difference_in_capacity;
    }
  }//resize-DONE


  void push_front(T x){ // push x in front, move front to the "left", resize
                        // if necessary.
    if (size >= capacity){resize();}//resize if not much space left
        // for (int i = size; i > front; i--){
        //   Q[size]=move[Q[left_index(size)]];
        // }//to make sure there is space before front, removed cause solved in resize
    if (size == 0){
      Q[front] = move(x);
      size++;
    }
    else{
      Q[left_index(front)] = move(x);
      front = left_index(front);
      size++;
    }
  }//push_front-DONE


  void push_back(T x){ // push value at back (right) end of queue
    if (size >= capacity){resize();}
    uint rear = (front+size-1)%capacity;
    Q[right_index((front+size-1)%capacity)] = move(x);//to the right of rear
    //Q[right_index[index[size-1]]]=x;
    size++;
  }//push_back-DONE
  

  T pop_back(){  // removes and returns value at end of queue, may
                 // throw exception
    if(size==0){throw "nothing to pop";}
    T answer = move(Q[(front + size -1)% capacity]);
    size--;
    return answer;
  }//pop_back-DONE


  T& operator [] (uint i){ // returns reference to value at logical index i.
  // This operation should have 0 overhead (no if-checks).
    T &x = Q[index(i)];
    return x;
  }//operator.done?


  bool is_sorted(){ // determines if Q is sorted in inceasing order
    if (size<=1){return true;}
    uint rear =(front+size-1)%capacity;
    int i = front;
    while(i != (rear)){
      if (Q[i] > Q[right_index(i)]){return false;}
      i = right_index(i);
    }
    return true;
  }//is_sorted-DONE


  // insert x into Q in sorted order, assuming Q is already sorted, returning
  // the *logical* index where it was inserted (0 to size-1).
  uint insert_ord(T x){
  // hint: the easy way to implement this is to temporarily push x
  // in front of the Q, then swap it into place, like in bubblesort.
  // you can call std::swap.
    push_front(x);
    uint rear =(front+size-1)%capacity;
    uint x_index = front;
    uint i = front;
    while(i != (rear)){
      if (Q[x_index] > Q[right_index(i)]){
        std::swap(Q[x_index],Q[right_index(i)]);
        x_index = right_index(i);
      }
      i = right_index(i);
    }
    return x_index;
  }//insert_ord-DONE


  uint reposition(uint i){ // repositions item at logical index i until
  // it's in sorted position.  This function should be called when a Q
  // value is changed, such as via operator []. Swap the item left or right.
  // The function should return the new logical index of the item.  If i
  // is invalid, throw an exception.
    if(size==0){throw "nothing to order";}
    if((i>size-1) || (i<0)){throw "invalid position";}
    i = index(i);
    uint rear =(front+size-1)%capacity;

    while(right_index(i)!=right_index(rear)){
      if(Q[i]>Q[right_index(i)]){
        std::swap(Q[i],Q[right_index(i)]);
        i = right_index(i);
      }else{
        //return i; // need to convert real to logical
        return (i-front)% capacity;
      }
    }
    return -1;
  }//reposition-DONE


  void insert_at(uint i, T x){ // insert x at logical index i, pushing
  // original value and all other values to the right.  Temporarily place
  // x at either front or end and swap until it's in place.
  // If i is larger than the size of the array, place it at the end.
  // To be efficient, you should swap it in the direction with fewer values.
    i = index(i);
    //cout<<"real index: "<<i<<endl;
    //for(int i=0;i<capacity;i++) cout << Q[i] << " ";  cout << endl;
    if ((size+1) >= capacity){resize();}
    uint rear =(front+size-1)%capacity;
    uint k = rear;

    while(k != (left_index(i))){
      Q[right_index(k)] = move(Q[k]);
      k = left_index(k);
    }//shift over all things after i position

    Q[i] = move(x);//after shifting the rest
    size++;
  }//insert_at-DONE
  

  T remove_at(uint i){// remove value at logical index i, throws exception
  // if index is invalid. Swap it to the left or right until it can be popped.
  // To be efficient, you should swap it in the direction with fewer values.
    i = index(i);
    uint rear =(front+size-1)%capacity;
    if(size==0){throw "nothing to remove";}
    bool front_then_rear = (front<=rear);
    if (front_then_rear && (!((i>=front) && (i <= rear)))){throw "invalid position";}
    if (!(front_then_rear) && ((i>=front) && (i <= rear))){throw "invalid position";}
    uint k = i;
    while (k != rear){
      std::swap(Q[k],Q[right_index(k)]);
      k = right_index(k);
    }
    T answer = pop_back();
    return answer;
  }//remove_at-DONE

  void truncate(unsigned int newsize){
    if(size >= newsize){
      size = newsize;
    }
  }//TRUNCATE
  

  bool binary_search(T& x){  // determines if x is in the Queue using
                             // binary search (possible since Q is an array)
    uint value = bin_search_upper(x);
    return (x== Q[index(value)]);
  }//binary_search-DONE, or can copy bin search upper and remove check at the bottom 

  uint bin_search_upper(T& x){ // returns either the logical index of where
  // x was found, or the index of the closest value that's greater than x.
  // For example, if you're searching for 5 in [2,4,7,8,9], it should return
  // 2, the index of 7, the closest value larger than 5.  If you search for 
  // 10 it should return 5, which is the size of the Q, indicating that
  // there's no value in the Q that's larger.
  bool weirdB = false; // never changes
  uint rear =(front+size-1)%capacity;
  uint left = front;
  uint right = rear;

  if(left > right){weirdB = true;}
  else{weirdB = false;}
   
  bool weirdA = weirdB;
  int mid;//GIVES REAL INDEX ISSUEEEEE
  int bid;//right index of mid
   
  bool hasFound = false;
  int target = x;
  while((weirdA == weirdB) && !hasFound){
    // get middle
    if(weirdA){mid = (((left + left + right + 1)/2) % capacity);}
    else{mid = (left + right)/2;}
       
    // Where is target compared to mid?
    if(target == Q[mid]){hasFound = true;}
    // on left
    if(target < Q[mid]){
      right = (mid + capacity - 1) % capacity; // Right = mid - 1
    }
    
    if(target > Q[mid]){
      left = ((mid + 1) % capacity);
    }
       
    // Re-check if its weirdA, they swap sides
    if(left > right){weirdA = true;}
    else{weirdA = false;}
       
    }
  if(hasFound){//return mid
    return (mid-front)% capacity;
  }
  else{//return right_index(mid)
    return (right_index(mid) - front)%capacity;
  }
  }//bin_search_upper_DONE

}; // circqueue

/*
  All functions must be implemented as efficiently as possible.  Push
  operations must run in amortized O(1) time.  Pop operations and
  operator [] must run in worst-case O(1) time.  is_sorted, insert_sorted,
  and remove should run in O(n) time.  The binary search operations 
  must be O(log n).  There should also be no superflous code.
*/

//uncomment when circqueue type has been completely implemented
/*
int main() {
  
  circqueue<int> q(1);
  for(int i=1;i<=5;i++) {q.push_front(i*10);  q.push_back(i*11);}
  q.insert_at(3,1000);
  q.insert_at(8,1111);
  for(int i=0;i<q.len();i++) cout << q[i] << " ";  cout << endl;
  cout << q.remove_at(2) << " (removed)\n";
  for(int i=0;i<q.len();i++) cout << q[i] << " ";  cout << endl;
  cout << q.remove_at(8) << " (removed)\n";
  for(int i=0;i<q.len();i++) cout << q[i] << " ";  cout << endl;
  
  // checking sorted queue
  circqueue<int> s(1);
  for(int i:{5,4,10,2,20,3,1,8,12,6}) s.insert_ord(i);
  for(int i=0;i<s.len();i++) cout << s[i] << " ";  cout << endl;
  int x = 7;
  int & rx = x;
  cout << s.binary_search(rx) << endl;
  s[2] = 9;
  s.reposition(2);
  s.insert_ord(11);
  for(int i=0;i<s.len();i++) cout << s[i] << " ";  cout << endl;
  int y = 9;
  int & ry = y;
  cout << s.binary_search(ry) << endl;
  int z = 11;
  int & rz = z;
  cout << s.bin_search_upper(rz)<<endl;  
  cout << s.is_sorted() << endl;
  return 0;
}//main
*/

