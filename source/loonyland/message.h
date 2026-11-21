#ifndef MESSAGE_H
#define MESSAGE_H

#include <string_view>
#include "winpch.h"
#include "display.h"

void InitMessage(void);
void NewMessage(std::string_view txt,int time);
void NewBigMessage(std::string_view txt,int time);
void UpdateMessage(void);
void RenderMessage(void);
void NoRepeatNewMessage(std::string_view txt,int time);

#endif
