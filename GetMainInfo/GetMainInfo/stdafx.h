#pragma once

// System Include
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#ifndef IMPORTS_H_
#define IMPORTS_H_

#define MAX_CATEGORY 16
#define MAX_ITEM_PER_CATEGORY 512
// Total de itens por categoria: MAX_ITEM_PER_CATEGORY (512)
// Total geral possível (categorias × itens por categoria): MAX_CATEGORY * MAX_ITEM_PER_CATEGORY (16 * 512 = 8192)
#define GET_ITEM(x,y) (x*MAX_ITEM_PER_CATEGORY+y) + 819
#define GET_ITEMN(x,y) (x*MAX_ITEM_PER_CATEGORY+y)

#define STRING_CRIPT(str) (char*)(str)


BOOL DirectoryExists(LPCTSTR path);

#endif