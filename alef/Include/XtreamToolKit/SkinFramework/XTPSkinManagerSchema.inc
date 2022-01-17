#undef BEGIN_TM_SCHEMA
#undef BEGIN_TM_PROPS
#undef BEGIN_TM_ENUM
#undef BEGIN_TM_CLASS_PARTS
#undef BEGIN_TM_PART_STATES
#undef TM_PROP
#undef TM_PART
#undef TM_STATE
#undef TM_ENUM
#undef END_TM_CLASS_PARTS
#undef END_TM_PART_STATES
#undef END_TM_PROPS
#undef END_TM_ENUM
#undef END_TM_SCHEMA
#undef TMSCHEMA_H

#define BEGIN_TM_SCHEMA(name)
#define BEGIN_TM_PROPS()
#define BEGIN_TM_ENUM(name)
#define BEGIN_TM_CLASS_PARTS(name)
#define BEGIN_TM_PART_STATES(name)
#define TM_PART(val, prefix, name)
#define TM_STATE(val, prefix, name)
#define TM_ENUM(val, prefix, name)
#define TM_PROP(val, prefix, name, primval)

#define END_TM_CLASS_PARTS()
#define END_TM_PART_STATES()
#define END_TM_PROPS()
#define END_TM_ENUM()
#define END_TM_SCHEMA(name)
