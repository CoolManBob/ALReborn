//<title Lex Schemas>
//<toctitle LexSchemas>
//<keywords lexClass>
//
// Summary:
//  *   Lex class description *
//
//  <B>lexClass</B> object is a block of text with defined begin and end sequence;
//  for which can be specified some font and colors attributes.
//  One lex class can contain others inside itself;
//  also it can have some relations to other classes.
//
//  lex schema is defined as a set of lexClasses.
//  It contains a root class with parent type 'file'
//  (which defines a files extensions to apply this schema to,
//  and some global schema attributes).
//
//  - Lex schemes -
//
//  Lex schemes are defined in *.schclass files.
//  Lex class properties (attributes) can be compound, like:
//  <P><C>parent:dyn, previous:tag:separators.</C>
//
//  - Multiple values -
//
//  For multiple value properties the following syntax is allowed:
//
//  <P><C>  propertyX = value1, value2, value3, .... </C>
//  <P><C>  propertyX = valueA1, valueA2, ... </C>
//  <P><C>  propertyX = valueN1 </C>
//  <P><C>  propertyX = valueN2 </C>
//  <P><C>          .... </C>
//
//  - String values -
//
//  String(character) values are defined in single quotation marks:
//  <P>'stringValue'<P>
//  There are a few special chars (as in C++):
//  <P>'\\', '\'', '\t', '\r', '\n'
//
//  And predefined constants:
//
//  <P><C> @alpha   = a-z, A-Z </C>
//  <P><C> @digit   = 0-9 </C>
//  <P><C> @HexDdigit = 0-9, a-f, A-F </C>
//  <P><C> @specs   = ~`!@#$%^&*()_-+=\|{}[];:'",.<>/? </C>
//  <P><C> @EOL     = End Of Line </C>
//
//  <P><P>NOT operation supported for constants comparisons,
//  <P>like: <C> @alpha:not </C>
//
//  - Comments -
//  <B> <COLOR 00c000> // </COLOR> </B>
//  <P>Two slashes are used to define a single line comment in Lex schema files.
//  (same as in C++)
//
//  - Lex Class Section -
//  <P>
//  <B>lexClass:</B>
//  <P>This keyword begins a lex class section.
//
//  - name \- Property (required) -
//
//  Define a lexClass name. Must be unique withing same lex schema (*.schclass file).
//  <PRE>
//  lexClass:
//      name = c_CPPString
//  </PRE>
// <P>
//
//  - parent \- Property (required) -
//
//  <B>parent </B>
//  <P>Defines a direct parent of lex class. (parent class name)
//
//  <P><B>parent:dyn</B>
//  <P>Defines a dynamic parent of lex class. (parent class name)
//  Dynamic means that parent class can be not only direct parent,
//  but parent of its parent [of parent ...] until the top class/block.
//
//  <P><B>parent:file</B>
//  <P>Defines a top schema class. (as file extensions list <*.ext1|*.ext2>)
//  <P>
//  <P>Only one of mentioned above 'parent' properties can be used for each class.
//
//  <PRE>
//  parent:file = <*.c|*.cpp|*.h|*.inl|*.tli|*.tlh|*.rc|*.rc2>
//  parent      = c_CPP
//  parent:dyn  = c_CPP
//  </PRE>
//
//  - children \- Property (optional) -
//
//  <B>children</B>
//  <P>Defines an allowed direct children for lex class
//  (as comma separated class names or zero to restrict any child).
//
//  <P>Parent-child relation is defined by 'parent' property.  This property helps
//  to restrict create parent-child relation for classes other than specified
//  or define that class has no children.
//
//  <PRE>
//  children = 0 // has no children
//  children = class1, class2, ...
//  </PRE>
//
//  - RecurrenceDepth \- Property  (optional) -
//
//  <B>RecurrenceDepth</B>
//  <P>Defines maximum depth of nested classes (blocks).
//  <P>(as a number, default value 1).
//  <P>
//  <P>Class A can be parent for class B. And Class B can be parent for class A.
//  Possible situation when A=B. (for example '{}' block in C++)
//  This property defines maximum depth of A->B->A->B->... nested construction.
//
//  <PRE>
//  RecurrenceDepth = 32
//  </PRE>
//
//  - previous \- Property (optional) -
//
//  'previous' property used to set condition to start lex class as previous
//  tag or class. The difference from 'start' property that objects specified
//  in this property is not included in class(block).
//  Can be used alone or together with 'start' property.
//
//  <P><B>previous:class</B>
//  <P>This class is started only if previous determined class name in collection
//  specified in this property.
//  <P>(as one or more class names)
//
//  <P><B>previous:tag</B>
//  <P>This class is started only if start condition is true and previous tag is
//  in collection specified in this property.
//  <P>(as one or more tags)
//
//  <P><B>previous:tag:separators</B>
//  <P>separators set for previous:tag values.
//
//  <P>Previous 'tag' and 'class' conditions can be specified separately or together.
//
//  <PRE>
//  previous:class  = c_HTMLelement_SCRIPT_lang_VB, c_HTMLelement_SCRIPT_lang_JS
//  previous:tag            = '='
//  previous:tag:separators = ' ', '\t', @eol
//  </PRE>
//
//  - start \- Property  (optional) -
//
//  'start' property used to set condition to start lex class as tag or class.
//  The difference from 'previous' property that objects specified in this
//  property is included in class(block).
//  <P>Can be used alone or together with 'previous' property.
//
//  <P><B>start:class</B>
//  <P>This class is started only if previous determined class name in collection
//  specified in this property.
//  <P>(as one or more class names)
//
//  <P><B>start:Tag</B>
//  <P>This class is started if current tag is in collection specified in this property.
//  <P>(as one or more tags)
//
//  <P>Start 'tag' and 'class' conditions can be specified separately or together.
//
//  - end \- Property (optional) -
//
//  'end' property used to set end lex class condition as tag or class.
//
//  <P><B>end:class</B>
//  <P>This class is ended if previous determined class name in collection
//  specified in this property.
//  <P>(as one or more class names)
//
//  <P><B>end:Tag</B>
//  <P>This class is ended if current tag is in collection specified in this property.
//  <P>(as one or more tags)
//
//  <P><B>end:separators</B>
//  <P>This class is ended if tag after current position is in collection specified
//  in this property. Difference from end:Tag is that end:Tag specify tags before
//  current position and end:Tag included in this block but end:separators not
//  included.
//  <P>(as one or more tags)
//
//  <P>End 'class', 'tag' and 'separators' conditions can be specified separately or together.
//
//  - skip \- Property  (optional) -
//
//  <B>skip:Tag</B>
//  <P>Used to skip some tags from parsing. Useful to support special tags
//  part of which can be a begin of end tag.
//  <P>(as one or more tags)
//
//  <PRE>
//  EXAMPLES (for start, end, skip):
//
//  start:Tag   ='/*'
//  end:Tag     ='*/'
//
//  //-------------------------------------------
//  start:Tag       = '"'
//  skip:Tag        = '\\"', '\\\r\n', '\\\n\r', '\\\n', '\\\r'
//  end:Tag         = '"', @eol
//
//  //-------------------------------------------
//  start:tag   = @alpha
//  skip:tag    = @digit
//  end:separators = @alpha:not
//
//  //-------------------------------------------
//  start:CLASS = c_CPPAfxInsertBlock_Start
//  end:CLASS   = c_CPPAfxInsertBlock_End
//  </PRE>
//
//  - token \- Property  (optional) -
//
//  'token' property is used to set lex class as some token(s).
//  It could be useful to define keywords.
//  Can be used alone or together with 'previous' property.
//  It must not be used together with 'start'-'end' condition.
//
//  <P><B>token:tag</B>
//  <P>This class is a word from this collection.
//  <P>(as one or more tags)
//
//  <P><B>Token:start:separators</B>
//  <P>Start separators set for token:tag values.
//
//  <P><B>Token:end:separators</B>
//  <P>End separators set for token:tag values.
//
//  <P>Token 'tag' is main condition. Token 'separators' additional conditions.
//  <P>'separators' without 'tag' have no sense.
//
//  <PRE>
//  token:start:separators  = ' ', '\t', '<%=', '<%', @specs, @eol
//  token:end:separators    = ' ', '\t', '%>', @specs, @eol
//
//  token:tag = 'if', 'for', 'while', 'do'
//  token:tag = 'break'
//  token:tag = 'case'
//  token:tag = 'catch'
//  </PRE>
//  <P>
//
//  * TEXT Attributes *
//
//  txt:XXX defines a few text properties for lex class (block of text).
//  All of them are optional and if some text attribute is not specified -
//  it is inherited from parent class or set to default value for a top (file) lex class.
//
//  <P><B>txt:colorFG</B>
//      <P>Fore color value in hex format (like 0xffc0c0).
//      <P>default: black
//
//  <P><B>txt:colorBK</B>
//      <P>Background color value in hex format (like 0xffFFff).
//      <P>default: white
//
//  <P><B>txt:colorSelFG</B>
//      <P>Selected text fore color value in hex format (like 0xffFFff).
//      <P>default: white
//
//  <P><B>txt:colorSelBK</B>
//      <P>Selected text background color value in hex format (like 0xab0077).
//      <P>default: black
//
//  <P><B>txt:Bold</B>
//      <P>Bold font attribute. Values {1, 0}
//      <P>default: 0
//
//  <P><B>txt:Italic</B>
//      <P>Italic font attribute. Values {1, 0}
//      <P>default: 0
//
//  <P><B>txt:Underline</B>
//      <P>Underline font attribute. Values {1, 0}
//      <P>default: 0
//
//  <PRE>
//  txt:colorFG = 0x0fc0c0
//  txt:colorBK = 0xffffff
//
//  txt:colorSelFG  = 0xffffff
//  txt:colorSelBK  = 0x0fc0c0
//
//  txt:Bold    = 1
//  txt:Italic  = 0
//  txt:Underline = 1
//  </PRE>
//
//  * caseSensitive \- Property  (optional) *
//
//  <B>caseSensitive</B>
//  <P>Define compare method for this lex class tags: case sensitive or insensitive.
//  <P>Inheritable from parent class.
//  <P>Values {1, 0}
//  <P>default: 0
//
//  * Collapsable \- Property  (optional) *
//
//  <B>Collapsable</B>
//  <P>Define is this block Collapsable or not. Inheritable from parent class.
//  <P>Values {1, 0}
//  <P>default: 0
//
//  * CollapsedText \- Property  (optional) *
//
//  <B>CollapsedText</B>
//  <P>Collapsed block string (as quoted string value).
//  <P>default: '[..]'
//
//  * ParseOnScreen \- Property  (optional) *
//
//  <B>ParseOnScreen</B>
//  <P>Values {1, 0}
//  <P>default: 0
//  <P>If set to 1 - lex class will be parsed only before drawing on the screen in
//  the main thread. Useful for keywords and other single line text blocks, which
//  are parsed very fast.
//
//  - \*\*\* Global attributes \*\*\* -
//
//  'global' attributes let to adjust some common parser parameters for each
//  lex schema separately. They can be used only in top layer lex class
//  (parent type file).
//
//  <P><B>global:FirstParseInSeparateThread</B>
//  <P>Values {1, 0}
//  <P>default: 1
//  <P>Define is first parse (when file opening) will run in the main or
//  additional thread.
//
//  <P><B>global:EditReparceInSeparateThread</B>
//  <P>Values {1, 0}
//  <P>default: 1
//  <P>Define is reparse after text edited will run in the main or
//  additional thread.
//
//  <P><B>global:ConfigChangedReparceInSeparateThread</B>
//  <P>Values {1, 0}
//  <P>default: 1
//  <P>Define is reparse text after schema file (*.schclass) will run
//  in the main or additional thread.
//
//  <P><B>global:EditReparceTimeout_ms</B>
//  <P>as time in milliseconds;
//  <P>default: 500
//  <P>Time out to start reparse after last key was pressed.
//
//  <P><B>global:MaxBackParseOffset</B>
//  <P>as number (chars);
//  <P>default: 100
//  <P>Some times parser look back for the text from current position.
//  This parameter define maximum back buffer size.
//
//  <P><B>global:OnScreenSchCacheLifeTime_sec</B>
//  <P>as time in seconds; -1 and 0 means infinite;
//  <P>default= 180
//  <P>When piece of text is displayed, edit control run parser to parse
//  lexClasses with 'ParseOnScreen' attribute set and cache this
//  information to do not run parser every time for this lines. For each
//  entry in this cache last access time is stored.
//  This parameter define time period from last access time after which
//  cached entries will be removed (for memory using optimization).
//
//  <P><B>global:ParserThreadIdleLifeTime_sec</B>
//  <P>as time in seconds; -1 and 0 means infinite;
//  <P>default: 60
//  <P>Idle time out for parser thread. If no parse requests (no editing operations)
//  additional parser thread will be exited. It will be recreated again
//  when necessary (to system resources optimization).
