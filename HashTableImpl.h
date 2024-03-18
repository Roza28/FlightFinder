#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include "IntPair.h"

//=======================//
// Implemented Functions //
//=======================//
template<int MAX_SIZE>
int HashTable<MAX_SIZE>::PRIMES[3] = {102523, 100907, 104659};

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintLine(int tableIndex) const
{
    const HashData& data = table[tableIndex];

    // Using printf here it is easier to format
    if(data.sentinel == SENTINEL_MARK)
    {
        printf("[%03d]         : SENTINEL\n", tableIndex);
    }
    else if(data.sentinel == EMPTY_MARK)
    {
        printf("[%03d]         : EMPTY\n", tableIndex);
    }
    else
    {
        printf("[%03d] - [%03d] : ", tableIndex, data.lruCounter);
        printf("(%-5s) ", data.isCostWeighted ? "True" : "False");
        size_t sz = data.intArray.size();
        for(size_t i = 0; i < sz; i++)
        {
            if(i % 2 == 0)
                printf("[%03d]", data.intArray[i]);
            else
                printf("/%03d/", data.intArray[i]);

            if(i != sz - 1)
                printf("-->");
        }
        printf("\n");
    }
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintTable() const
{
    printf("____________________\n");
    printf("Elements %d\n", elementCount);
    printf("[IDX] - [LRU] | DATA\n");
    printf("____________________\n");
    for(int i = 0; i < MAX_SIZE; i++)
    {
        PrintLine(i);
    }
}

//=======================//
//          TODO         //
//=======================//
template<int MAX_SIZE>
int HashTable<MAX_SIZE>::Hash(int startInt, int endInt, bool isCostWeighted)
{
    /* TODO */
    int hashcode;
    int cost = (isCostWeighted ? 1 : 0);
    hashcode = (PRIMES[0]*startInt + PRIMES[1]*endInt + PRIMES[2]*cost) % MAX_SIZE;
    return hashcode;
}

template<int MAX_SIZE>
HashTable<MAX_SIZE>::HashTable()
{
    /* TODO */
    for (int i = 0; i < MAX_SIZE;i++){
        table[i].sentinel = EMPTY_MARK;
        table[i].lruCounter = 0; //?//
        elementCount = 0;
    }
}

template<int MAX_SIZE>
int HashTable<MAX_SIZE>::Insert(const std::vector<int>& intArray, bool isCostWeighted)
{
    /* TODO */
    if (intArray.size() < 1)
        throw InvalidTableArgException();
    int size = intArray.size();
    int startInt = intArray[0]; int endInt = intArray[size - 1];
    
    int hashcode = Hash(startInt,endInt,isCostWeighted);
    int tmp = hashcode;
    int i = 1;
    while (table[tmp].sentinel != EMPTY_MARK && table[tmp].sentinel != SENTINEL_MARK){
        if (table[tmp].startInt == startInt && table[tmp].endInt == endInt && table[tmp].isCostWeighted == isCostWeighted)
            return table[tmp].lruCounter++;
        tmp = (hashcode + i*i) % MAX_SIZE;
        i++;
    }

    if (elementCount == (MAX_SIZE + 1)/2)
        throw TableCapFullException(elementCount);
        
    table[tmp].sentinel = OCCUPIED_MARK;
    table[tmp].intArray = intArray;
    table[tmp].isCostWeighted = isCostWeighted;
    table[tmp].startInt = startInt;
    table[tmp].endInt = endInt;
    table[tmp].lruCounter = 1;
    elementCount++;
    return 0;

}

template<int MAX_SIZE>
bool HashTable<MAX_SIZE>::Find(std::vector<int>& intArray,
                               int startInt, int endInt, bool isCostWeighted,
                               bool incLRU)
{
    /* TODO */
    int hashcode = Hash(startInt,endInt,isCostWeighted);
    int tmp = hashcode;
    int i = 1;
    while (table[tmp].sentinel != EMPTY_MARK){
        if (table[tmp].sentinel == SENTINEL_MARK)
            continue;
        if (table[tmp].startInt == startInt && table[tmp].endInt == endInt && table[tmp].isCostWeighted == isCostWeighted){
            if (incLRU)
                table[tmp].lruCounter++;
            intArray = table[tmp].intArray;
            return true;
        }
        tmp = (hashcode + i*i) % MAX_SIZE;
        i++;
    }
    return false;
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::InvalidateTable()
{
    /* TODO */
    for (int i = 0; i < MAX_SIZE;i++){
        if (table[i].sentinel == EMPTY_MARK)
            continue;
        if (table[i].sentinel == SENTINEL_MARK)
            table[i].sentinel = EMPTY_MARK;
        
        else{
            table[i].sentinel = EMPTY_MARK;
            table[i].intArray.clear();
        }
        
    }
    elementCount = 0;    
    
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::GetMostInserted(std::vector<int>& intArray) const
{
    /* TODO */
    int max = -1;
    int index;
    for (int i = 0; i < MAX_SIZE;i++){
        if (table[i].sentinel != EMPTY_MARK && table[i].sentinel != SENTINEL_MARK){
            if (table[i].lruCounter > max){
                max = table[i].lruCounter;
                index = i;
            }
        }
    }
    intArray = table[index].intArray;
    
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::Remove(std::vector<int>& intArray,
                                 int startInt, int endInt, bool isCostWeighted)
{
    /* TODO */
    int hashcode = Hash(startInt,endInt,isCostWeighted);
    int tmp = hashcode;
    int i = 1;
    while (table[tmp].sentinel != EMPTY_MARK){
        if (table[tmp].sentinel == SENTINEL_MARK)
            continue;
        if (table[tmp].startInt == startInt && table[tmp].endInt == endInt && table[tmp].isCostWeighted == isCostWeighted){
            intArray = table[tmp].intArray;
            table[tmp].intArray.clear();
            table[tmp].sentinel = SENTINEL_MARK;
            elementCount--;
        }
        tmp = (hashcode + i*i) % MAX_SIZE;
        i++;
    }
    return;
    
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::RemoveLRU(int lruElementCount)
{
    /* TODO */
    if (lruElementCount > elementCount)
        return;
    MinPairHeap<int,int> pq;
    for (int i = 0; i < MAX_SIZE;i++){
        if (table[i].sentinel != EMPTY_MARK && table[i].sentinel != SENTINEL_MARK){
            if (table[i].lruCounter != 0){
                Pair<int,int> pair;
                pair.key = table[i].lruCounter;
                pair.value = i;
                pq.push(pair);
            }
        }
    }

    while (lruElementCount > 0){
       int index = pq.top().value;
       table[index].intArray.clear();
       table[index].sentinel = SENTINEL_MARK;
       elementCount--;
       pq.pop();
       lruElementCount--;
    }
    
    
    
    
    
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintSortedLRUEntries() const
{
    /* TODO */
    MaxPairHeap<int,int> pq;
    for (int i = 0; i < MAX_SIZE;i++){
        if (table[i].sentinel != EMPTY_MARK && table[i].sentinel != SENTINEL_MARK){
            if (table[i].lruCounter != 0){
                Pair<int,int> pair;
                pair.key = table[i].lruCounter;
                pair.value = i;
                pq.push(pair);
            }
        }
    }
    while (!pq.empty()){
        PrintLine(pq.top().value);
        pq.pop();
    }

}


#endif // HASH_TABLE_HPP