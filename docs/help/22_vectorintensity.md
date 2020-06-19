##### Vector To Intensity Map Node

![vectorintensity](images/vectorintensity.png)

Converts an input [rgba](28_types.md) map to a [mono](28_types.md) map using a selected method of channel selection.

---

**Ports**

The node has **1** input and **1** output port(s).

- **Input 1** (*rgba*): The input vector map.
- **Output 1** (*mono*): The resulting [mono](28_types.md) map converted from the input map.

---

The options available are:

**Red**: Output only the red channel.

**Green**: Output only the green channel.

**Blue**: Output only the blue channel.

**Alpha**: Output only the alpha channel.

**Average**: Average the channels together *(r + g + b + a) / 4*.

**Min**: Selects the smallest of the four channels.

**Max**: Selects the largest of the four channels.