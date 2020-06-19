# Colour Split Node

![color split](images/colorsplit.png)

The colour split node takes in a single [rgba](types.md) map and splits each of the channel out into its own [mono](types.md) channel.

---

**Ports**

The node has **1** input and **4** output port(s).

- **Input 1** (*rgba*): The [rgba](types.md) input to be split out.
- **Output 1** (*red*): The [mono](types.md) out for the red channel.
- **Output 2** (*green*): The [mono](types.md) out for the green channel.
- **Output 3** (*blue*): The [mono](types.md) out for the blue channel.
- **Output 4** (*alpha*): The [mono](types.md) out for the alpha channel.