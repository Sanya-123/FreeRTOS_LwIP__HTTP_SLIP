/**
******************************************************************************
* @file           avltree.c
* @author         ¹ÅÃŽÄþ
* @brief          avltree file. Æœºâ¶þ²æÊ÷µÄÊµÏÖ
******************************************************************************
*
* COPYRIGHT(c) 2018 GoodMorning
*
******************************************************************************
*/
/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>

#include "avltree.h"


//#define AVL_DEBUG

#ifdef AVL_DEBUG
	#define avl_debug(...) printf(__VA_ARGS__)
#else
	#define avl_debug(...)
#endif





static void __avl_rotate_right(struct avl_node *node, struct avl_root *root);
static void __avl_rotate_left(struct avl_node *node, struct avl_root *root);

static int __avl_balance_right(struct avl_node *node, struct avl_root *root);
static int __avl_balance_left(struct avl_node *node, struct avl_root *root);

static int __left_hand_insert_track_back(struct avl_node *node, struct avl_root *root);
static int __right_hand_insert_track_back(struct avl_node *node, struct avl_root *root);


#define   __left_hand_delete_track_back(node,root)   __right_hand_insert_track_back(node,root) 
#define   __right_hand_delete_track_back(node,root)   __left_hand_insert_track_back(node,root) 



/**
	* @brief    __avl_rotate_right
	*           ¶Ô node œÚµãœøÐÐµ¥ÓÒÐýŽŠÀí
	* @param    node        ¶þ²æÊ÷œÚµã
	* @param    root        ¶þ²æÊ÷žù
	* @return   
*/
static void __avl_rotate_right(struct avl_node *node, struct avl_root *root)
{
    //¶ÔÒÔ*nodeÎªžùµÄ¶þ²æÊ÷×÷ÓÒÐý×ªŽŠÀí£¬ŽŠÀíÖ®ºónodeÖžÏòÐÂµÄÊ÷žùœáµã
    //ŒŽÐý×ªŽŠÀíÖ®Ç°µÄ×ó×ÓÊ÷µÄžùœáµã
    struct avl_node *left = node->avl_left;
    struct avl_node *parent = avl_parent(node);

    if ((node->avl_left = left->avl_right))
        avl_set_parent(left->avl_right, node);
	
    left->avl_right = node;

    avl_set_parent(left, parent);

    if (parent)
    {
        if (node == parent->avl_right)
            parent->avl_right = left;
        else
            parent->avl_left = left;
    }
    else
        root->avl_node = left;
	
    avl_set_parent(node, left);
}


/**
* @brief    __avl_rotate_left
*           ¶Ô node œÚµãœøÐÐµ¥×óÐýŽŠÀí
* @param    node        ¶þ²æÊ÷œÚµã
* @param    root        ¶þ²æÊ÷žù
* @return
*/
static void __avl_rotate_left(struct avl_node *node, struct avl_root *root)
{
    struct avl_node *right = node->avl_right;
    struct avl_node *parent = avl_parent(node);

    if ( (node->avl_right = right->avl_left) )
        avl_set_parent(right->avl_left, node);
	
    right->avl_left = node;

    avl_set_parent(right, parent);

    if (parent)
    {
        if (node == parent->avl_left)
            parent->avl_left = right;
        else
            parent->avl_right = right;
    }
    else
        root->avl_node = right;
	
    avl_set_parent(node, right);
}


/**
* @brief    __avl_balance_left
*           µ± node œÚµã×ó×ÓÊ÷žßÓÚÓÒ×ÓÊ÷£¬ ¶Ô node œÚµãœøÐÐ×óÆœºâŽŠÀí²¢žüÐÂÆœºâÒò×Ó
* @param    node        ¶þ²æÊ÷œÚµã
* @param    root        ¶þ²æÊ÷žù
* @return   ¶ÔÓÚÔ­ node ËùÔÚÎ»ÖÃ£¬Ÿ­¹ýÆœºâŽŠÀíÊ¹Ê÷µÄžß¶ÈœµµÍÁË·µ»Ø -1£¬·ñÔò·µ»Ø0
*/
static int __avl_balance_left(struct avl_node *node, struct avl_root *root)
{
	int retl = 0;
	int left_right_child_scale;
    struct avl_node * left_child = node->avl_left;
    struct avl_node * left_left_child ;
    struct avl_node * left_right_child;
	
    if(left_child)
	{
		left_left_child = left_child->avl_left;
		left_right_child = left_child->avl_right;
		
		
		switch(avl_scale(left_child))
		{
			case AVL_BALANCED://Ö»ÓÐÔÚÉŸ³ýµÄÊ±ºò»á³öÏÖÕâÖÖÇé¿ö£¬Çé¿ö·Ç³£žŽÔÓ£¬ÐèÐ¡ÐÄŽŠÀí		
				left_right_child_scale = avl_scale(left_right_child);
				__avl_rotate_left(node->avl_left, root); //¶Ô*nodeµÄ×ó×ÓÊ÷×÷×óÆœºâŽŠÀí
				__avl_rotate_right(node, root);          //¶Ô*node×÷ÓÒÆœºâŽŠÀí
				avl_set_tilt_left(left_child);
				avl_set_tilt_left(left_right_child);
				
				if (left_right_child_scale == AVL_BALANCED)
					avl_set_balanced(node);
				else 
				if (left_right_child_scale == AVL_TILT_LEFT)
					avl_set_tilt_right(node);
				else
				{
					int left_left_child_scale = avl_scale(left_left_child) ;
					avl_set_balanced(node);
					retl = __avl_balance_left(left_child, root);//ÕâÖÖÇé¿öÐèµÝ¹é,²¢ÐèÒªžùŸÝµÝ¹éœá¹ûžüÐÂÆœºâÒò×Ó
					if ((retl < 0)||(left_left_child_scale != AVL_BALANCED))
						avl_set_balanced(left_right_child);
				}
				avl_debug("L_R\r\n");
				break;

			case AVL_TILT_LEFT:
				__avl_rotate_right(node,root);
				avl_set_balanced(node);
				avl_set_balanced(left_child);
				retl = -1;//žß¶È±äµÍÁË
				avl_debug("R\r\n");
				break;
				
			case AVL_TILT_RIGHT:
				__avl_rotate_left(node->avl_left,root);  //¶Ô*nodeµÄ×ó×ÓÊ÷×÷×óÆœºâŽŠÀí
				__avl_rotate_right(node,root);      //¶Ô*node×÷ÓÒÆœºâŽŠÀí
				
				switch(avl_scale(left_right_child))
				{
					case AVL_BALANCED:
						avl_set_balanced(node);
						avl_set_balanced(left_child);
						break;
					case AVL_TILT_RIGHT:
						avl_set_balanced(node);
						avl_set_tilt_left(left_child);
						break;
					case AVL_TILT_LEFT:
						avl_set_balanced(left_child);
						avl_set_tilt_right(node);
						break;
				}
				
				avl_set_balanced(left_right_child);
				retl = -1;//žß¶È±äµÍ
				
				avl_debug("L_R\r\n");
				break;
		}
	}
	
	return retl;
}



/**
* @brief    __avl_balance_right
*           µ± node œÚµãÓÒ×ÓÊ÷žßÓÚ×ó×ÓÊ÷£¬ ¶Ô node œÚµãœøÐÐ×óÆœºâŽŠÀí²¢žüÐÂÆœºâÒò×Ó
* @param    node        ¶þ²æÊ÷œÚµã
* @param    root        ¶þ²æÊ÷žù
* @return   ¶ÔÓÚÔ­ node ËùÔÚÊ÷µÄÎ»ÖÃ£¬Ÿ­¹ýÆœºâŽŠÀíÊ¹Ê÷µÄžß¶ÈœµµÍÁË·µ»Ø -1£¬·ñÔò·µ»Ø0
*/
static int __avl_balance_right(struct avl_node *node, struct avl_root *root)
{
	int ret = 0;
	int right_left_child_scale;
    struct avl_node * right_child = node->avl_right;
    struct avl_node * right_left_child = right_child->avl_left;
	struct avl_node * right_right_child = right_child->avl_right;
	
    if(right_child)
	{
		switch(avl_scale(right_child))
		{
			case AVL_BALANCED://ÉŸ³ýµÄÊ±ºò»á³öÏÖÕâÖÖÇé¿ö£¬ÐèÒªÌØ±ð×¢Òâ
				right_left_child_scale = avl_scale(right_left_child);
				
				__avl_rotate_right(node->avl_right,root);
				__avl_rotate_left(node,root);
				avl_set_tilt_right(right_left_child);
				avl_set_tilt_right(right_child);
				
				if (right_left_child_scale == AVL_BALANCED)
					avl_set_balanced(node);
				else 
				if (right_left_child_scale == AVL_TILT_RIGHT)
					avl_set_tilt_left(node);
				else
				{
					int right_right_child_scale = avl_scale(right_right_child);
					avl_set_balanced(node);
					ret = __avl_balance_right(right_child, root);//ÐèµÝ¹éÒ»ŽÎ
					if ((ret < 0)||(right_right_child_scale != AVL_BALANCED)) 
						avl_set_balanced(right_left_child);
				}	
				avl_debug("R_L\r\n");
				break;

			case AVL_TILT_RIGHT:
				avl_debug("L\r\n");
				__avl_rotate_left(node, root);
				avl_set_balanced(node);
				avl_set_balanced(right_child);
				ret = -1;//žß¶È±äµÍÁË//
				break;

			case AVL_TILT_LEFT:
				__avl_rotate_right(node->avl_right,root);
				__avl_rotate_left(node,root);
	
				avl_debug("R_L\r\n");
				switch(avl_scale(right_left_child)) //Ðý×ªºóÒªžüÐÂÆœºâÒò×Ó
				{
					case AVL_TILT_LEFT:
						avl_set_tilt_right(right_child);
						avl_set_balanced(node);
						break;
					case AVL_BALANCED:
						avl_set_balanced(node);
						avl_set_balanced(right_child);
						break;
					case AVL_TILT_RIGHT:
						avl_set_balanced(right_child);
						avl_set_tilt_left(node);
						break;
				}
				
				avl_set_balanced(right_left_child);
				ret = -1;//
				break;
		}
	}
	
	return ret;
}



/**
* @brief    __left_hand_insert_track_back
*           ÔÚ node œÚµãµÄ×ó×ÓÊ÷œøÐÐÁË²åÈë£¬žüÐÂÆœºâÒò×Ó£¬ÈçÊ§ºâÔò×÷ÆœºâŽŠÀí
* @param    node        ¶þ²æÊ÷œÚµã
* @param    root        ¶þ²æÊ÷žù
* @return   ¶ÔÓÚÔ­ node ËùÔÚÊ÷µÄÎ»ÖÃ£¬Ÿ­¹ýÆœºâŽŠÀíÊ¹Ê÷µÄžß¶ÈœµµÍÁË·µ»Ø -1£¬·ñÔò·µ»Ø0
*/
static int __left_hand_insert_track_back(struct avl_node *node, struct avl_root *root)
{
	switch(avl_scale(node))
	{
		case AVL_BALANCED  :
			avl_set_tilt_left(node);
			return 0;
			
		case AVL_TILT_RIGHT:
			avl_set_balanced(node);
			return -1;
			
		case AVL_TILT_LEFT :
			return __avl_balance_left(node,root);
	}
	
	return 0;
}



/**
* @brief    __left_hand_insert_track_back
*           ÔÚ node œÚµãµÄÓÒ×ÓÊ÷œøÐÐÁË²åÈë£¬žüÐÂÆœºâÒò×Ó£¬ÈçÊ§ºâÔò×÷ÆœºâŽŠÀí
* @param    node        ¶þ²æÊ÷œÚµã
* @param    root        ¶þ²æÊ÷žù
* @return   ¶ÔÓÚÔ­ node ËùÔÚÊ÷µÄÎ»ÖÃ£¬²åÈë²¢Æœºâºóžß¶ÈœµµÍÁË·µ»Ø -1£¬·ñÔò·µ»Ø0
*/
static int __right_hand_insert_track_back(struct avl_node *node, struct avl_root *root)
{
	switch(avl_scale(node))
	{
		case AVL_BALANCED:
			avl_set_tilt_right(node);//žžœÚµãÓÒÇã
			return 0;                //ÒÔ node ÎªžùµÄÊ÷žß¶È±»žÄ±ä£¬µ«ÎŽÊ§ºâ
			
		case AVL_TILT_LEFT:
			avl_set_balanced(node);//žžœÚµãÆœºâ
			return -1;             //ÒÔ node ÎªžùµÄÊ÷žß¶È²»žÄ±ä
			
		case AVL_TILT_RIGHT://ÒÔ node ÎªžùµÄÊ÷ÒÑÊ§ºâ£¬×÷ÆœºâŽŠÀí
			return __avl_balance_right(node,root);//
	}
	
	return 0;
}




void avl_insert(
	struct avl_root *root,
	struct avl_node * insertnode,
	struct avl_node * parent,
	struct avl_node ** avl_link)
{
	int taller = 1;

	struct avl_node *gparent = NULL;

	uint8_t parent_gparent_path = 0;
	uint8_t backtrack_path = 0;

	insertnode->avl_parent = (unsigned long)parent;
	insertnode->avl_left = insertnode->avl_right = NULL;

	*avl_link = insertnode;

	if (root->avl_node == insertnode) return;

	if (AVL_BALANCED != avl_scale(parent))
	{
		avl_set_balanced(parent);//žžœÚµãÆœºâ
		return;//Ê÷Ã»³€žß·µ»Ø
	}

	backtrack_path = (insertnode == parent->avl_left) ? AVL_TILT_LEFT : AVL_TILT_RIGHT;

	//Ê÷³€žßÁË£¬ÐèÒª»ØËÝÆœºâ
	while (taller && parent)
	{
		//»ØËÝÆœºâ¹ý³Ì»ážÄ±äÊ÷µÄœá¹¹£¬ÏÈŒÇÂŒ×æžžœÚµãºÍ¶ÔÓŠµÄ»ØËÝÂ·Ÿ¶·œÏò
		if ( (gparent = avl_parent(parent)) )//ÏÈž³ÖµÔÙÅÐ¶Ï
			parent_gparent_path = (parent == gparent->avl_right) ? AVL_TILT_RIGHT : AVL_TILT_LEFT;

		if (backtrack_path == AVL_TILT_RIGHT)
			taller += __right_hand_insert_track_back(parent, root);
		else
			taller += __left_hand_insert_track_back(parent, root);

		backtrack_path = parent_gparent_path; //»ØËÝ
		parent = gparent;
	}
}

#if 1
void avl_delete(struct avl_root *root, struct avl_node * node)
{
	struct avl_node *child, *parent;
	struct avl_node *gparent = NULL;
	int lower = 1;

	uint8_t parent_gparent_path = 0;
	uint8_t backtrack_path = 0;

	if (!node->avl_left)//Èç¹û±»ÉŸœÚµã²»ŽæÔÚ×ó×ÓÊ÷
	{
		child = node->avl_right; //°ÑÓÒ×ÓÊ÷œÓÈëžžœÚµãÖÐ

		parent = avl_parent(node);

		if (child) avl_set_parent(child, parent);

		if (parent)
		{
			if (parent->avl_left == node)
			{
				parent->avl_left = child;
				backtrack_path = AVL_TILT_LEFT;
			}
			else
			{
				parent->avl_right = child;
				backtrack_path = AVL_TILT_RIGHT;
			}
		}
		else
		{
			root->avl_node = child;
			if (child) avl_set_balanced(child);
			return;
		}
	}
	else if (!node->avl_right) //Èç¹û±»ÉŸœÚµãŽæÔÚ×ó×ÓÊ÷£¬²»ŽæÔÚÓÒ×ÓÊ÷
	{
		child = node->avl_left;

		parent = avl_parent(node);

		avl_set_parent(child, parent);

		if (parent)
		{
			if (parent->avl_left == node)
			{
				parent->avl_left = child;
				backtrack_path = AVL_TILT_LEFT;
			}
			else
			{
				parent->avl_right = child;
				backtrack_path = AVL_TILT_RIGHT;
			}
		}
		else
		{
			root->avl_node = child;
			if (child) avl_set_balanced(child);
			return;
		}
	}
	else //±»ÉŸœÚµãŒŽŽæÔÚ×ó×ÓÊ÷£¬Ò²ŽæÔÚÓÒ×ÓÊ÷
	{
		struct avl_node *old = node, *left;

		node = node->avl_right;

		while ((left = node->avl_left) != NULL) node = left;//ÕÒµœÓÒ×ÓÊ÷ÏÂ×îÐ¡µÄÌæŽúµã

		if (avl_parent(old)) //ŸÉµãËùÔÚžžœÚµã
		{
			if (avl_parent(old)->avl_left == old)
				avl_parent(old)->avl_left = node;
			else
				avl_parent(old)->avl_right = node;

		}
		else root->avl_node = node;

		child = node->avl_right;  //×îÐ¡µÄÌæŽúµãµÄÓÒœÚµã
		parent = avl_parent(node);//×îÐ¡µÄÌæŽúµãµÄžžœÚµã

		if (parent == old) //ÒªÉŸ³ýµÄœÚµãµÄÓÒ×ÓÊ÷Ã»ÓÐ×ó×ÓÊ÷£¬ÌæŽúµã(1)Ã»ÓÐÓÒœÚµã,(2)ŽæÔÚÒ»žöÓÒœÚµã  
		{
			backtrack_path = AVL_TILT_RIGHT;

			parent = node;
			node->avl_parent = old->avl_parent;
			node->avl_left = old->avl_left;
			avl_set_parent(old->avl_left, node);
		}
		else  //ÒªÉŸ³ýµÄœÚµãµÄÓÒ×ÓÊ÷ÓÐ×ó×ÓÊ÷
		{
			backtrack_path = AVL_TILT_LEFT;

			parent->avl_left = child;          //×îÐ¡µÄÌæŽúµãµÄÓÒœÚµãµÄžžœÚµã
			node->avl_right = old->avl_right;
			avl_set_parent(old->avl_right, node);

			node->avl_parent = old->avl_parent;
			node->avl_left = old->avl_left;
			avl_set_parent(old->avl_left, node);

			if (child) avl_set_parent(child, parent); //ÒªÉŸ³ýµÄœÚµãµÄÓÒ×ÓÊ÷µÄ×ó×ÓÊ÷Ä©Î²µãŽæÔÚÓÒœÚµã
		}
	}

	//Ê÷µÍÁË£¬ÐèÒª»ØËÝÆœºâ£¬Ö±µœ»ØËÝµœžùœÚµã
	while (lower && parent)
	{
		if ( (gparent = avl_parent(parent)) )//(parent && (gparent = avl_parent(parent)))//ÏÈž³ÖµÔÙÅÐ¶Ï
			parent_gparent_path = (parent == gparent->avl_right) ? AVL_TILT_RIGHT : AVL_TILT_LEFT;

		if (backtrack_path == AVL_TILT_RIGHT) //Ÿ­¹ý»ØËÝµ÷Õû»ážÄ±äÊ÷µÄœá¹¹£¬ËùÒÔÏÈŒÇÂŒ gparent ºÍ»ØËÝÂ·Ÿ¶ 
			lower = __right_hand_delete_track_back(parent, root);
		else
			lower = __left_hand_delete_track_back(parent, root);

		backtrack_path = parent_gparent_path;
		parent = gparent;
	}
}

#endif

/*
 * This function returns the first node (in sort oright_left_childer) of the tree.
 */
struct avl_node *avl_first(const struct avl_root *root)
{
    struct avl_node    *n;

    n = root->avl_node;
    if (!n)
        return NULL;
    while (n->avl_left)
        n = n->avl_left;
    return n;
}

struct avl_node *avl_last(const struct avl_root *root)
{
    struct avl_node    *n;

    n = root->avl_node;
    if (!n)
        return NULL;
    while (n->avl_right)
        n = n->avl_right;
    return n;
}

struct avl_node *avl_next(const struct avl_node *node)
{
    struct avl_node *parent;

    if (avl_parent(node) == node)
        return NULL;

    /* If we have a right-hand child, go down and then left as far
       as we can. */
    if (node->avl_right) {
        node = node->avl_right; 
        while (node->avl_left)
            node=node->avl_left;
        return (struct avl_node *)node;
    }

    /* No right-hand children.  Everything down and left is
       smaller than us, so any 'next' node must be in the general
       direction of our parent. Go up the tree; any time the
       ancestor is a right-hand child of its parent, keep going
       up. First time it's a left-hand child of its parent, said
       parent is our 'next' node. */
    while ((parent = avl_parent(node)) && node == parent->avl_right)
        node = parent;

    return parent;
}

struct avl_node *avl_prev(const struct avl_node *node)
{
    struct avl_node *parent;

    if (avl_parent(node) == node)
        return NULL;

    /* If we have a left-hand child, go down and then right as far
       as we can. */
    if (node->avl_left) {
        node = node->avl_left; 
        while (node->avl_right)
            node=node->avl_right;
        return (struct avl_node *)node;
    }

    /* No left-hand children. Go up till we find an ancestor which
       is a right-hand child of its parent */
    while ((parent = avl_parent(node)) && node == parent->avl_left)
        node = parent;

    return parent;
}

void avl_replace_node(struct avl_node *victim, struct avl_node *new,
             struct avl_root *root)
{
    struct avl_node *parent = avl_parent(victim);

    /* Set the surrounding nodes to point to the replacement */
    if (parent) {
        if (victim == parent->avl_left)
            parent->avl_left = new;
        else
            parent->avl_right = new;
    } else {
        root->avl_node = new;
    }
    if (victim->avl_left)
        avl_set_parent(victim->avl_left, new);
    if (victim->avl_right)
        avl_set_parent(victim->avl_right, new);

    /* Copy the pointers/colour from the victim to the replacement */
    *new = *victim;
}
