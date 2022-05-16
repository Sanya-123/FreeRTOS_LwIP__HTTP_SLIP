#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__

//------------------------------------------------------------------
#ifndef offsetof
	#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)// »ñÈ¡"MEMBER³ÉÔ±"ÔÚ"œá¹¹ÌåTYPE"ÖÐµÄÎ»ÖÃÆ«ÒÆ
#endif

#ifndef container_of
	#if 1
		// žùŸÝ"œá¹¹Ìå(type)±äÁ¿"ÖÐµÄ"Óò³ÉÔ±±äÁ¿(member)µÄÖžÕë(ptr)"ÀŽ»ñÈ¡ÖžÏòÕûžöœá¹¹Ìå±äÁ¿µÄÖžÕë
		#define container_of(ptr, type, member)  ((type*)((char*)ptr - offsetof(type, member)))
		// ŽËºê¶šÒåÔ­ÎÄÎª GNU C ËùÐŽ£¬ÈçÏÂ£¬ÓÐÐ©±àÒëÆ÷Ö»Ö§³Ö ANSI C /C99 µÄ£¬ËùÒÔ×÷ÒÔÉÏÐÞžÄ
	#else
		#define container_of(ptr, type, member) ({          \
		const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		(type *)( (char *)__mptr - offsetof(type,member) );})
	#endif
#endif
//------------------------------------------------------------------

struct avl_node
{
    unsigned long  avl_parent; 
    struct avl_node *avl_left;
    struct avl_node *avl_right;
};


struct avl_root
{
    struct avl_node *avl_node;
};


#define avl_parent(r)         ((struct avl_node *)((r)->avl_parent & (~3)))

//------------------------------------------------------------------
//»ñÈ¡ avl œÚµãµÄÆœºâÒò×Ó£¬ÖµÈçÏÂ
#define avl_scale(r)        ((r)->avl_parent & 3) 
#define AVL_BALANCED   0 //œÚµãÆœºâ
#define AVL_TILT_RIGHT 1 //œÚµãÓÒ±ß±È×ó±ßžß£¬ÓÃ 0b01 ±íÊŸ£¬ÓÒÇã
#define AVL_TILT_LEFT  2 //œÚµã×ó±ß±ÈÓÒ±ßžß£¬ÓÃ 0b10 ±íÊŸ£¬×óÇã


//------------------------------------------------------------------
//ÉèÖÃ avl œÚµãµÄÆœºâÒò×Ó
//ÉèÖÃ avl œÚµãÎªÆœºâœÚµã
#define avl_set_balanced(r)   do {((r)->avl_parent) &= (~3);}while(0)

//ÉèÖÃ avl œÚµãÎªÓÒÇãœÚµã
#define avl_set_tilt_right(r) do {(r)->avl_parent=(((r)->avl_parent & ~3)|AVL_TILT_RIGHT);} while (0)

//ÉèÖÃ avl œÚµãÎª×óÇãœÚµã
#define avl_set_tilt_left(r)  do {(r)->avl_parent=(((r)->avl_parent & ~3)|AVL_TILT_LEFT);} while (0)

//------------------------------------------------------------------


#define  avl_entry(ptr, type, member) container_of(ptr, type, member)



// ÉèÖÃ avl œÚµãµÄžžœÚµãÎª p
static inline void avl_set_parent(struct avl_node *avl, struct avl_node *p)
{
    avl->avl_parent = (avl->avl_parent & 3) | (unsigned long)p;
}



void avl_insert(
               struct avl_root *root,
               struct avl_node * insertnode, 
               struct avl_node * insertparent,
			   struct avl_node ** avl_link);

void avl_delete(struct avl_root *root,struct avl_node * node);
			   
/* Find logical next and previous nodes in a tree */
extern struct avl_node *avl_next(const struct avl_node *);
extern struct avl_node *avl_prev(const struct avl_node *);
extern struct avl_node *avl_first(const struct avl_root *);
extern struct avl_node *avl_last(const struct avl_root *);
void avl_replace_node(struct avl_node *victim, struct avl_node *new,
             struct avl_root *root);
#endif
