#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include <map>

struct InputState{    
	InputState(): numKeysPressed(0), lMousePressed(false), rMousePressed(false), prevX(0), prevY(0), deltaX(0), deltaY(0) {}

	//is a key being held down?
	int numKeysPressed;
	std::map<int, bool> keyMap;
    // Is the mouse button currently being held down?
	bool lMousePressed;
	bool rMousePressed;
    // Last known position of the cursor
	float prevX;
	float prevY;
    // Accumulated change in cursor position. 
	float deltaX;
	float deltaY;

	void updateKeyboard(int key, bool pressed){
		//keep track of num keys pressed
		if(pressed){
			numKeysPressed++;
		} else {
			numKeysPressed--;
		}
		//update map
		keyMap[key] = pressed;
	}
    // Update cursor variables based on new position x,y
    void updateMouse(float x, float y){
        float xDiff = x - prevX;
        float yDiff = y - prevY;
        deltaX += xDiff;
        deltaY += yDiff;
        prevX = x;
        prevY = y;
    };

    // Read off the accumulated motion and reset it
    void readDeltaAndReset(float *x, float *y){
        *x = deltaX;
        *y = deltaY;
        deltaX = 0;
        deltaY = 0;
    };
};

#endif
