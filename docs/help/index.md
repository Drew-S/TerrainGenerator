# Overview

The main application is setup into 4 distinct areas, the **preview** panel, the **properties** panel, the **dataflow** panel, and the **project settings** panel.

![application](images/application.png)

## [Preview Panel](preview.md)

![preview](images/application-preview-mask.png)

The preview panel displays the output of the generated height map that is piped into the output node in the dataflow diagram.

## [Editor Panel](editor.md)

![editor](images/application-nodeeditor-mask.png)

The editor panel is divided into two components, the properties panel: that can be used to update selected nodes properties; and the dataflow panel: that is used to connect the nodes and create terrain generation.

### [Properties Panel](properties.md)

![properties](images/application-properties-mask.png)

The properties panel shows a copy of controls of a selected node. The values are synced with the embedded nodes properties and can be updated from here.

### [Dataflow Panel](dataflow.md)

![dataflow](images/application-dataflow-mask.png)

The dataflow panel is where the terrain generation is done from. The nodes here connect to define the procedure to generate the terrain.

## [Project Settings Panel](settings.md)

![dataflow](images/application-settings-mask.png)

The settings panel houses project specific settings (separate from application settings in the menubar), these settings are preview and output resolution of images, the preview window colours, and other settings.

