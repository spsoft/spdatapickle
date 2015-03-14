# What is it? #

SPDataPickle is a library use to serialize and deserialize C structures to and from XML/JSON/ProtoBuf

# Quick Example #

You write a .xml file like this:
```
<metainfo prefix="XYZ" filename="account">  
    <struct name="Person">  
        <field name="id"    type="int32" />  
        <field name="name"  type="*char" />  
        <field name="email" type="*char" />  
    </struct>  
</metainfo> 
```

Then you process it with spxml2struct, the SPDataPickle tools, to produce code in pure C.
```
enum {  
    eTypeXYZPerson = eTypeSPDPUserDefine + 1   
};  
  
typedef struct tagXYZPerson {  
    int mId;  
    char * mName;  
    char * mEmail;  
} XYZPerson_t;  
  
typedef struct tagSP_DPMetaInfo SP_DPMetaInfo_t;  
extern SP_DPMetaInfo_t * gXYZAccountMetaInfo;  
```

Then, you use that code like this:
```
XYZPerson_t person;  

person.mId = 123;  
person.mName = strdup( "Bob" );  
person.mEmail = strdup( "bob@example.com" );  

SP_XmlStringBuffer buffer;  

SP_XmlPickle pickle( gXYZAccountMetaInfo );  
pickle.pickle( &person, sizeof( person ), eTypeXYZPerson, &buffer );  

printf( "%s\n", buffer.getBuffer() );  

XYZPerson_t other;  
pickle.unpickle( buffer.getBuffer(), buffer.getSize(),  
        eTypeXYZPerson, &other, sizeof( other ) );  

printf( "id %d, name %s, email %s\n", other.mId, other.mName, other.mEmail );  

SP_DPAlloc alloc( gXYZAccountMetaInfo );  

alloc.free( &person, sizeof( person ), eTypeXYZPerson );  
alloc.free( &other, sizeof( other ), eTypeXYZPerson );
```

```
<Person>  
<id>123</id>  
<name>Bob</name>  
<email>bob@example.com</email>  
</Person>  
  
id 123, name Bob, email bob@example.com 
```