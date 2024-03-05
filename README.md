# Cymbols
## A Source Code Analyser Helps You To Understand Code Efficiently

Cymbols is a source code analyzer that tries implementing software called ["Source Insignht"](https://www.sourceinsight.com/)(a classical Windows software) on MacOS started in 2018. There is no reason to continue this project when Microsoft's VS Code becomes increasingly populated because VS Code implements all the features that Cymbols tries to do.
Now, Cymbols has become an open-source project to help people who want to learn how to write a text editor and how to use Flex to do computer program lexical analysis. I hope you enjoy it.

### Source Architecture
  - cee_backend: database operations.
  - cee_binary: binary content display and edit.
  - cee_lib: libraries of base algorithms such as list/regex operations,  memory management, etc.
  - cee_parser: languages parsers implemented in Flex.
  - cee_text: source content display and edit.
  - All UI-related components source is in path "Cymbols/Cymbols".


![](https://raw.githubusercontent.com/yuqingcai/Cymbols/7d789abfd785bcf56d73f4393b12e3634f4769cb/Resources/Prompt/screen_shot.svg)


### Features:


#### Symbol Parse
  Cymbols has built-in source parsers. It parses source code into symbols and creates the source outline.
  
![](https://raw.githubusercontent.com/yuqingcai/Cymbols/7d789abfd785bcf56d73f4393b12e3634f4769cb/Resources/Prompt/symbol_list.svg)
  
  Target Languages:
  - C	Supported
  - C++	Supported
  - Objective C	Supported
  - HTML	Supported
  - CSS	Supported
  - Java	Supported
  - GNU Assembly	Supported
  - C#	Supported
  - JavaScript	Not Yet
  - Swift	Not Yet


#### Context Parse
Cymbols analyze the source context when you navigate codes. Source context may generally be a function, variable, or custom data definition. If a definition is duplicated, Cymbols will list them as a table and let you make a selection.

![](https://raw.githubusercontent.com/yuqingcai/Cymbols/a3832cc20bbb8bd013d8676cdcef083dcc2d7726/Resources/Prompt/context.svg)


#### Source Navigate
If a source file is parsed, you can go to the declaration or definition of a symbol with the command shortcut "CMD + Click."  If the declaration or definition is duplicated, they are listed as a table and let you make a selection.

![](https://raw.githubusercontent.com/yuqingcai/Cymbols/a3832cc20bbb8bd013d8676cdcef083dcc2d7726/Resources/Prompt/source_navigate.svg)


#### Source Edit
An editor is contained in Cymbols. It supports syntax highlight and nested split layout; a befitting layout may speed up your code review.

![](https://raw.githubusercontent.com/yuqingcai/Cymbols/a3832cc20bbb8bd013d8676cdcef083dcc2d7726/Resources/Prompt/split_layout.svg)


#### Source navigation
If a source file is parsed, you can go to the declaration or definition of a symbol with CMD + Click; if the declaration or definition is duplicated, they are listed as a table and let you make a selection.

![](https://raw.githubusercontent.com/yuqingcai/Cymbols/a3832cc20bbb8bd013d8676cdcef083dcc2d7726/Resources/Prompt/source_navigate.svg)


#### Reference Search
Reference Search is another critical function in Cymbols. When a project is built, right-click a symbol and select the "Search Reference..." item in the popup menu; all references from the emblem will be listed, and clicking an item will jump to the reference.

![](https://raw.githubusercontent.com/yuqingcai/Cymbols/a3832cc20bbb8bd013d8676cdcef083dcc2d7726/Resources/Prompt/reference_search.svg)


#### Multiple UI Themes
Cymbols's style look can be customized. By default, we provide 2 UI Themes, "Cymbols" and "Monokai." You can create your own style to make a new look.

![](https://raw.githubusercontent.com/yuqingcai/Cymbols/a3832cc20bbb8bd013d8676cdcef083dcc2d7726/Resources/Prompt/multiple_theme.svg)



## Screen Shot
![](https://github.com/yuqingcai/Cymbols/blob/master/Resources/Prompt/prompt.gif)


