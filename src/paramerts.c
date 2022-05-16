#include "paramerts.h"
#include "lwip/apps/snmp_scalar.h"

typedef struct {
    uint8_t id;
    char *parametsrName;
    bool write;
    snmp_err_t (*setParam) (struct snmp_node_instance* instance, u16_t len, void *value);
    //получилось ли записать            какойто указатель           размер входных данныч  входные данные
    /*
    snmp_err_t exampleSet(struct snmp_node_instance* instance, u16_t len, void *value)
    {
        for(int i = 0; i < len; i++)
            mas[i] = value[i];
        return SNMP_ERR_NOERROR;
    }
    */
    s16_t (*readParam) (struct snmp_node_instance* instance, void* value);
    //размер выходных данных           какойто указатель        куда писать данные
    /*
    s16_t exampleRead(struct snmp_node_instance* instance, void* value)
    {
        u32_t *uint_ptr = (u32_t*) value;
        *uint_ptr = data;
        return sizeof(*uint_ptr);
    }
    */
    uint8_t typeReturn;
}ParametStruct;

ParametStruct findStructByID(uint8_t id);

//***************name***************
s16_t retuenName(struct snmp_node_instance* instance, void* value)
{
//    if(instance->reference_len == 0)
//        instance->reference.ptr = findStructByID(0).parametsrName;
//    instance->reference_len = 4;
    LWIP_UNUSED_ARG(instance);
    MEMCPY(value, "150MHz", 6);
    return 6;
}
//******************************


//***************POWER***************
uint8_t power = 159;

snmp_err_t setPover(struct snmp_node_instance* instance, u16_t len, void *value)
{
    LWIP_UNUSED_ARG(instance);
    LWIP_UNUSED_ARG(len);
    power = *((uint8_t*)value);
    return SNMP_ERR_NOERROR;
}

s16_t readPower(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*) value;
    *uint_ptr = power;
    return sizeof(*uint_ptr);
}
//******************************

ParametStruct nullParam = {0, "", false, NULL, NULL};

ParametStruct masParam[] = {
    {0, "Name", false, NULL, retuenName, SNMP_ASN1_TYPE_OCTET_STRING},
    {1, "Power", true, setPover, readPower, SNMP_ASN1_TYPE_INTEGER}
};

#define SIZE_MASPARAM  (sizeof(masParam)/sizeof(masParam[0]))

ParametStruct findStructByID(uint8_t id)
{
    for(int i = 0; i < SIZE_MASPARAM; i++)
        if(masParam[i].id == id)
            return masParam[i];
    return nullParam;
}

//инициализация параметров в SNMP *******НЕ ТРОГАТЬ*******
struct snmp_scalar_node scalarNode[SIZE_MASPARAM];
struct snmp_node* my_nodes_parametrs[SIZE_MASPARAM];
struct snmp_tree_node meTreeNodeParametrs = SNMP_CREATE_TREE_NODE(1, my_nodes_parametrs);
const u32_t mib2_my_oid_arr[] = { 2, 7, 40 };
struct snmp_mib myMibsParametrs = SNMP_MIB_CREATE(mib2_my_oid_arr, &meTreeNodeParametrs.node);

void initParam()
{
    for(int i = 0; i < SIZE_MASPARAM; i++)
    {
        struct snmp_scalar_node temp1 = SNMP_SCALAR_CREATE_NODE(i + 1, SNMP_NODE_INSTANCE_READ_WRITE,\
                                masParam[i].typeReturn, masParam[i].readParam, NULL, masParam[i].setParam);
        struct snmp_scalar_node temp2 = SNMP_SCALAR_CREATE_NODE_READONLY( i + 1, masParam[i].typeReturn, masParam[i].readParam);
        if(masParam[i].write)
            scalarNode[i] = temp1;
        else
            scalarNode[i] = temp2;

        my_nodes_parametrs[i] = &scalarNode[i].node.node;
    }
}
//**************

