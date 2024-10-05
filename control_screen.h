#ifndef INTERFACE_H
#define INTERFACE_H


void print_interface()
{
	gotoxy(163, 2);
	printf("");
	gotoxy(163, 3);
	printf("                                  ");
	gotoxy(163, 4);
	printf("             Circle               ");
	gotoxy(163, 5);
	printf("                                  ");
	gotoxy(163, 6);
	printf("");
	gotoxy(163, 7);
	printf("");
	gotoxy(163, 8);
	printf("                                  ");
	gotoxy(163, 9);
	printf("            Triangle              ");
	gotoxy(163, 10);
	printf("                                  ");
	gotoxy(163, 11);
	printf("");
	gotoxy(163, 12);
	printf("");
	gotoxy(163, 13);
	printf("                                  ");
	gotoxy(163, 14);
	printf("             Square               ");
	gotoxy(163, 15);
	printf("                                  ");
	gotoxy(163, 16);
	printf("");
	gotoxy(163, 17);
	printf("");
	gotoxy(163, 18);
	printf("                                  ");
	gotoxy(163, 19);
	printf("            Pentagon              ");
	gotoxy(163, 20);
	printf("                                  ");
	gotoxy(163, 21);
	printf("");
	gotoxy(163, 22);
	printf("");
	gotoxy(163, 23);
	printf("                                  ");
	gotoxy(163, 24);
	printf("             Hexagon              ");
	gotoxy(163, 25);
	printf("                                  ");
	gotoxy(163, 26);
	printf("");
	
	gotoxy(163, 28);
	printf("");
	gotoxy(163, 29);
	printf("                                  ");
	gotoxy(163, 30);
	printf("           Screen Reset           ");
	gotoxy(163, 31);
	printf("                                  ");
	gotoxy(163, 32);
	printf("");

	gotoxy(163, 52);
	printf(" Friction ");
	gotoxy(163, 53);
	printf("                                  ");
	gotoxy(163, 54);
	printf("  ==============================  ");
	gotoxy(163, 55);
	printf("                                  ");
	gotoxy(163, 56);
	printf("");
	
	gotoxy(163, 58);
	printf(" SIZE ");
	gotoxy(163, 59);
	printf("                                  ");
	gotoxy(163, 60);
	printf("  ==============================  ");
	gotoxy(163, 61);
	printf("                                  ");
	gotoxy(163, 62);
	printf("");
	
	gotoxy(163, 64);
	printf("");
	gotoxy(163, 65);
	printf("                                  ");
	gotoxy(163, 66);
	printf("           Change Map 1           ");
	gotoxy(163, 67);
	printf("                                  ");
	gotoxy(163, 68);
	printf("");
}

class CONTROLLER
{
	public:
		CONTAINER &container;
		RENDERER &renderer;
		
		int fric = 0;
		
		int shape_size = 9;
		int shape_type{-1};
		
		const char *shape_name[5];
		
		HANDLE hInput;
		
	    INPUT_RECORD inputRecord;
	    DWORD events;
		
	public:
		CONTROLLER(CONTAINER &container, RENDERER &renderer)
		  :container(container), renderer(renderer)
		{
		    hInput = GetStdHandle(STD_INPUT_HANDLE);
		
		    DWORD prevMode;
		    GetConsoleMode(hInput, &prevMode);
		    SetConsoleMode(hInput, (prevMode | ENABLE_MOUSE_INPUT) & ~(ENABLE_QUICK_EDIT_MODE));
		    
		    shape_name[0] = "       Circle       ";
			shape_name[1] = "      Triangle      ";
		    shape_name[2] = "       Square       ";
			shape_name[3] = "      Pentagon      ";
		    shape_name[4] = "       Hexagon      ";
		    
			for (int n = 0; n < 70; ++n)
			{
				gotoxy(161,1+n);
				printf("|");
			}
		}
		
		void on()
		{
			setColor(GRAY, BLACK);
			print_interface();
			
			gotoxy(166 + fric*2, 54);
			printf("||");
			gotoxy(166 + (shape_size-2)*2, 60);
			printf("||");
		}
		
		void off()
		{
			setColor(DARKGRAY, BLACK);
			
			print_interface();

			gotoxy(166 + fric*2, 54);
			printf("||");
			gotoxy(166 + (shape_size-2)*2, 60);
			printf("||");
			
			setColor(GRAY, BLACK);
			
			shape_type = -1;
		}
		
		bool use()
		{
	        // 콘솔 입력 읽기 
	        ReadConsoleInput(hInput, &inputRecord, 1, &events);
	
	        if (inputRecord.EventType == MOUSE_EVENT)
	        {
	            MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;
	            
	            if (mouseEvent.dwEventFlags == 0) // 클릭 이벤트
	            {
	                if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
	                {
	                    // 좌측 버튼 클릭 이벤트 처리
	                    COORD pos = mouseEvent.dwMousePosition;
	                    
	                    pos.X += 1;
	                    pos.Y += 1;
	                    
	                    if (pos.X >= 166)
	                      return click_controller(pos);
	                    else
	                    {
	                    	add_shape(pos);
	                    	return true;
	                    }
	                }
	            }
	        }
	        
	        return false;
		}
		
		void add_shape(COORD &pos)
		{
			if (shape_type != -1)
			{
				Vector2f v{static_cast<float>(pos.X)/2.0f, static_cast<float>(pos.Y)};
				
				container.addShape(shape_type, v, shape_size);
			}
		}
		
		bool click_controller(COORD &pos)
		{
        	int click = -1;
        	
        	for (int n = 0; n < 5; ++n) // type
        	{
            	if (pos.Y >= 2 + n*5 && pos.Y <= 6 + n*5)
            	{
            		click = n;
            		
            		if (shape_type != click)
            		{
            			if (shape_type != -1)
            			{
                			setColor(GRAY, BLACK);
                			
                			gotoxy(170, 4 + shape_type*5 - 1);
                			printf("                    ");
                			gotoxy(170, 4 + shape_type*5);
                			printf("%s", shape_name[shape_type]);
                			gotoxy(170, 4 + shape_type*5 + 1);
                			printf("                    ");
            			}
            			
            			shape_type = click;
            			
            			setColor(BLACK, GRAY);

            			gotoxy(170, 4 + shape_type*5 - 1);
            			printf("                    ");		                    			
            			gotoxy(170, 4 + shape_type*5);
            			printf("%s", shape_name[shape_type]);
            			gotoxy(170, 4 + shape_type*5 + 1);
            			printf("                    ");
            			
            			setColor(GRAY, BLACK);
					}
            		
            		break;
            	}
        	}

        	if (pos.Y >= 53 && pos.Y <= 55) // friction
        	{
        		if (pos.X >= 166 && pos.X <= 195)
        		{
        			click = (pos.X-166) / 2;
        			
					gotoxy(166 + fric*2, 54);
					printf("==");
					
					fric = click;
					container.fric = 0.01f + fric * 0.1f;
					
					gotoxy(166 + fric*2, 54);
					printf("||");
				}
        	}
            	
        	else if (pos.Y >= 59 && pos.Y <= 61) // size
        	{
        		if (pos.X >= 166 && pos.X <= 195)
        		{
        			click = (pos.X-166) / 2;
        			
					gotoxy(166 + (shape_size-2)*2, 60);
					printf("==");
					
					shape_size = 2 + click;
					
					gotoxy(166 + (shape_size-2)*2, 60);
					printf("||");
				}
        	}
        	
        	else if (pos.Y >= 28 && pos.Y <= 32) // reset
        	{
        		container.reset();
        		return true;
        	}
        	
        	else if (pos.Y >= 64 && pos.Y <= 68) // map change
        	{
        		container.reset();
        		
        		if (container.constraint_type < 3)
				  container.constraint_type += 1;
				else
				  container.constraint_type = 0;
        		
        		gotoxy(186, 66);
        		printf("%d", 1 + container.constraint_type);
        		
        		if (container.constraint_type == 3)
        		{
        			std::vector<SHAPE*> vec;
        			
        			int radius = 1.0f;
        			
        			for (int n = 0; n < 30; ++n)
        			  vec.push_back(container.addShape(0, {10.0f + 2.0f*n, 47.0f}, radius));
        			for (int n = 0; n < 29; ++n)
        			  container.addLink(vec[n]->points[0], vec[n+1]->points[0], 2.2f);
					
					container.addPin(vec[0], vec[0]->points[0]->pos);
					container.addPin(vec[29], vec[29]->points[0]->pos);
				}
        		
        		return true;
        	}
			
			return false;
		}
		
		void draw_constraint()
		{				
			if (container.constraint_type == 0)
			  renderer.draw_circle({40.0f, 35.0f}, 35.0f);
			else if (container.constraint_type == 2)
		 	  renderer.draw_line({40.0f, 70.0f}, {80.0f, 50.0f});
		}
};











#endif






