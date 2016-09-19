---
layout: post
title:  "Finding smallest number without remainder from 1 to n"
date:   2016-06-11 03:43:45 +0700
categories: [python]
author: Gabor, Project Euler
---
author: **ProjectEuler**


2520 is the smallest number that can be divided by each of the numbers from 1 to 10 without any remainder.
What is the smallest positive number that is evenly divisible by all of the numbers from 1 to 20?

My Solution:

```python
import math
import timeit
start = timeit.default_timer()
def powers(num):
    factor = 2
    ans = []
    factors = []
    while num!=1:
        count = 0
        factors.sort()
        if num % factor ==0:
            while num % factor ==0:
                num/=factor
                count +=1
            if factor in factors:
                if factors[factors.index(factor)] < count:
                    ans[factors.index(factor)] = count
            else:
                factors.append(factor)
                ans.append(count)
        factor +=1
    return ans,factors
def multipows(num,factors,ans):
    factor = 2
    while num!=1:
        count = 0
        factors.sort()
        if num % factor ==0:
            while num % factor ==0:
                num/=factor
                count +=1
            if factor in factors:
                if factors[factors.index(factor)] < count:
                    ans[factors.index(factor)] = count
            else:
                factors.append(factor)
                ans.append(count)
        factor +=1
    return ans,factors
def prob5(num):
    factors = []
    ans = []
    a = 1
    for x in range(2,num+1):
        [factors,ans]= multipows(x,factors,ans)
        print(factors)
        print(ans)
    for factor in factors:
        a *= factor**ans[factors.index(factor)]
    return a
print(prob5(20))
stop = timeit.default_timer()
print stop - start 
```