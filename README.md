[中文版自述文件](Docs/README/README_zh-CN.md)

# Source Module Generator  

## Intro  
Source Module Generator is a Unreal Engine 4 plugin that supporting a simple way to create new module in project or plugin.  

## Features  
- Interactive interface dialog.  
- Generate new project module or plugin module.  
- Hot reload new generated module.  

## Install  
- ### From Source  
  1. Navigate to your plugin directory that included by game project direcoty.  
    `cd <GameProjectDirectoryPath>/Plugins`  
    Manually create directory named `Plugins` if it does not exist.  
  2. Clone source code from github.  
    `git clone https://github.com/hy741oo/SourceModuleGenerator.git`  
  3. Start game project by UE4 Editor to build the plugin.  
- ### From Binary  
  1. Download zip file from release package.  
  2. Unzip the file into plugin directory, manually create directory named "Plugins" if it does not exist.  
  3. Start geme project to use the plugin.  

## Usage
-  Open interactive dialog  
  ![HowToOpenInteractiveDialog](Docs/Images/HowToOpenInteractiveDialog.gif)
-  Enter Necessary Message  
  ![EnterNecessaryMessage](Docs/Images/EnterNecessaryMessage.png)
  1. Module Name: What module name do you want for new module will generated.  
  2. Is this a plugin module: If this box does not check, new module will be added into project, otherwise will be added into plugin that selected by following option.  
  3. Which plugin: If 2. box has been checked, new module will be added into plugin that selected by this option.  
-  Generate Module  
  ![GenerateModule](Docs/Images/GenerateModule.png)  
  1. Generate: The plugin will generate new module after clicked this button.  
