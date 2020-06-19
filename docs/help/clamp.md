# Clamp Node

![clamp](images/clamp.png)

The clamp node clamps the input node between two values, a minimum and maximum. The min and max can be overwritten with inputs which take the min and max at the corresponding pixels. The mode drop down has the option for *Clamp* which will truncate values beyond the min and max to the min and max, or *Sigmoid* which clamps the value using a smooth function to the min and max.

---

**Ports**

The node has **3** input and **1** output port(s).

- **Input 1** (*mono*): The input to be clamping.
- **Input 2** (*min*): The minimum value to use.
- **Input 3** (*max*): The maximum value to use.
- **Output 1** (*mono*): The resulting clamp.

---

The first option is a dropdown select which allows you to select either to *Clamp* (hard limit the value) or *Sigmoid* (smooth clamp the value).