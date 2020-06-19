##### Math Node

![math](images/math.png)

The math node applies a variety of mathematical operations on the two input [mono](28_types.md) maps producing a single output map.

---

**Ports**

The node has **2** input and **1** output port(s).

- **Input 1** (*mono*): The first value to use.
- **Input 2** (*mono*): The second value to use.
- **Output 1** (*mono*): The result of the operation.

---

The two numerical properties are defaults to be used if the input ports are not set.

The dropdown menu provides for a number of operations that can be applied on the values.

**Mix**: Blends the two values together *(a + b) / 2*.

**Add**: Adds the two values together *a + b*.

**Subtract**: Subtracts the bottom value from the top *a - b*.

**Multiply**: Multiplies the two values together *a * b*.

**Divide**: Divides the top value by the bottom value *a / b* (if *b* is 0 the resulting value is 0).

**Min**: Chooses the smallest of the two values.

**Max**: Chooses the largest of the two values.

**Pow**: Raises the top value by the bottom value *a^b*.
