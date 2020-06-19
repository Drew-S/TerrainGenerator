##### Slope and Curve Node

![bezier](images/bezier.png)

The slope and curve node converts an input value to a new output value following the formula of y = f(x), where f(x) is defined by the curve. To edit the curve simply click and drag the control points to adjust the curve. Double clicking on the curve will split the curve to add more detail. Control click a control point will snap it to an end point (does not do anything to an end point). Pressing x while hovering over a curve will delete the curve (1 curve will always remain).

---

**Ports**

The node has **1** input and **1** output port(s).

- **Input 1** (*mono*): The input is of type [mono](28_types.md).
- **Output 1** (*mono*): The output is of type [mono](28_types.md). The result of the transformation curve being applied on the input.

---

![click drag](icons/left_click_drag.png) &nbsp;&nbsp;&nbsp;Left click and drag a control point to adjust the curve.

![double click](icons/double_click.png) &nbsp;&nbsp;&nbsp;Double click on the curve will split the (sub)curve into two smaller curves that can be edited to add detail.

![ctrl click](icons/ctrl_click.png) &nbsp;&nbsp;&nbsp;Control clicking a control point will snap the control point to its matching end point. Creating a straight line.

![x](icons/x_click.png) &nbsp;&nbsp;&nbsp;Pressing x while hovering on a curve will delete the sub-curve. If only one curve remains it will not be deleted.