/*
 * Arduino_led_matrix_scrolling_text.cpp
 *
 * Created: 16.09.2012 12:40:53
 *  Author: Samuel (www.saduino.ch)
 */ 
 
// Text
char *Text = "This is an example text! 0123456789";
// write your own text on runtime over serial interface
char TextDynamic[256] = {0};
uint8_t TextLength = 0;
// text during runtime over serial interface
bool ShowDynamic = false;

// Settings: Display
const uint8_t ShowFreq = 1;     // LED refresh rate (ms)
const uint8_t ShowLength = 8;   // picture repetition rate (6 times)
const uint8_t ShowWait = 8;    // wait after picture (ms)

// Settings: Pins
const uint8_t NumberHIGH = 8;
const uint8_t NumberLOW = 8;
uint8_t pinHIGH[NumberHIGH] = {11,7,5,3,12,9,16,18};    // Pin assignment HIGH
uint8_t pinLOW[NumberLOW] = {10,6,4,2,13,8,17,19};      // Pin assignment LOW

// Prototypen
uint8_t get_character(char Character, uint8_t MatrixCharacter[]);
uint8_t scrolling_text(char *Text, const uint8_t ShowFreq, const uint8_t ShowLength, const uint8_t ShowWait, const uint8_t NumberHIGH, const uint8_t NumberLOW, uint8_t pinHIGH[NumberHIGH], uint8_t pinLOW[NumberLOW]);
uint8_t showMatrix(uint8_t LEDmatrix[], const uint8_t ShowFreq, const uint8_t ShowLength, const uint8_t ShowWait, const uint8_t NumberHIGH, const uint8_t NumberLOW, uint8_t pinHIGH[NumberHIGH], uint8_t pinLOW[NumberLOW]);
uint8_t animation(const uint8_t ShowFreq, const uint8_t ShowLength, const uint8_t ShowWait, const uint8_t NumberHIGH, const uint8_t NumberLOW, uint8_t pinHIGH[NumberHIGH], uint8_t pinLOW[NumberLOW]);

void setup()
{
    Serial.begin(9600);     // set serial interface
    
    // Set Pins
    for(int a=0; a<NumberHIGH; a++)     // set all high Pins to OUTPUT
        pinMode(pinHIGH[a], OUTPUT);
    for(int a=0; a<NumberHIGH; a++)     // set all high Pins to LOW
        digitalWrite(pinHIGH[a],LOW);
    for(int a=0; a<NumberLOW; a++)      // set all low Pins to OUTPUT
        pinMode(pinLOW[a], OUTPUT);
    for(int a=0; a<NumberLOW; a++)      // set all low Pins to HIGH
        digitalWrite(pinLOW[a],HIGH);
}
    
// start program
void loop()
{
    // little animation
    animation(ShowFreq, ShowLength, ShowWait, NumberHIGH, NumberLOW, pinHIGH, pinLOW);
    
    // scrolling text
    if (ShowDynamic)
    {
        // add new letters
        while (Serial.available() > 0)
        {
            TextDynamic[TextLength] = Serial.read();
            TextLength++;
        }
        // Print text serial and with LED matrix
        if (TextLength > 0)
        {
            for (int a=0; a<TextLength; a++)
            {
                Serial.println(TextDynamic[a]);
            }
            Serial.print("TextLength: ");   // control with serial output
            Serial.println(TextLength);     // 
    
            // show scrolling text on LED matrix
            // show your text from serial
            scrolling_text(TextDynamic, ShowFreq, ShowLength, ShowWait, NumberHIGH, NumberLOW, pinHIGH, pinLOW);
        
            // wait some time after the text
            delay(500);
        }
    }  
    else
    {
        // show scrolling text on LED matrix
        // your static text
        scrolling_text(Text, ShowFreq, ShowLength, ShowWait, NumberHIGH, NumberLOW, pinHIGH, pinLOW);
    }
}



// Functions
uint8_t get_character(char Character, uint8_t MatrixCharacter[])
{
// alphabet
uint8_t A[8] = {0x38, 0x44, 0x44, 0x7C, 0x44, 0x44, 0x44, 0x00}; 
uint8_t B[8] = {0x78, 0x44, 0x44, 0x78, 0x44, 0x44, 0x78, 0x00}; 
uint8_t C[8] = {0x78, 0x40, 0x40, 0x40, 0x40, 0x40, 0x78, 0x00}; 
uint8_t D[8] = {0x78, 0x44, 0x44, 0x44, 0x44, 0x44, 0x78, 0x00}; 
uint8_t E[8] = {0x78, 0x40, 0x40, 0x70, 0x40, 0x40, 0x78, 0x00}; 
uint8_t F[8] = {0x78, 0x40, 0x40, 0x70, 0x40, 0x40, 0x40, 0x00}; 
uint8_t G[8] = {0x78, 0x40, 0x40, 0x58, 0x48, 0x48, 0x78, 0x00}; 
uint8_t H[8] = {0x48, 0x48, 0x48, 0x78, 0x48, 0x48, 0x48, 0x00}; 
uint8_t I[8] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00}; 
uint8_t J[8] = {0x08, 0x08, 0x08, 0x08, 0x08, 0x48, 0x78, 0x00}; 
uint8_t K[8] = {0x48, 0x58, 0x70, 0x60, 0x70, 0x58, 0x48, 0x00}; 
uint8_t L[8] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x78, 0x00}; 
uint8_t M[8] = {0x44, 0x6C, 0x7C, 0x54, 0x44, 0x44, 0x44, 0x00}; 
uint8_t N[8] = {0x44, 0x64, 0x74, 0x54, 0x5C, 0x4C, 0x44, 0x00}; 
uint8_t O[8] = {0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00}; 
uint8_t P[8] = {0x70, 0x48, 0x48, 0x70, 0x40, 0x40, 0x40, 0x00}; 
uint8_t Q[8] = {0x38, 0x44, 0x44, 0x44, 0x44, 0x4C, 0x3E, 0x00}; 
uint8_t R[8] = {0x70, 0x48, 0x48, 0x70, 0x60, 0x50, 0x48, 0x00}; 
uint8_t S[8] = {0x78, 0x40, 0x40, 0x78, 0x08, 0x08, 0x78, 0x00}; 
uint8_t T[8] = {0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00}; 
uint8_t U[8] = {0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x38, 0x00}; 
uint8_t V[8] = {0x44, 0x44, 0x6C, 0x28, 0x38, 0x10, 0x10, 0x00}; 
uint8_t W[8] = {0x44, 0x44, 0x44, 0x44, 0x54, 0x7C, 0x28, 0x00}; 
uint8_t X[8] = {0x44, 0x6C, 0x38, 0x10, 0x38, 0x6C, 0x44, 0x00}; 
uint8_t Y[8] = {0x44, 0x6C, 0x38, 0x10, 0x10, 0x10, 0x10, 0x00}; 
uint8_t Z[8] = {0x7C, 0x0C, 0x18, 0x10, 0x30, 0x60, 0x7C, 0x00}; 
// special character
uint8_t questionmark[8] = {0x70, 0x08, 0x08, 0x30, 0x20, 0x00, 0x20, 0x00}; 
uint8_t exclamationmark[8] = {0x30, 0x30, 0x30, 0x30, 0x00, 0x30, 0x30, 0x00}; 
uint8_t dot[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00}; 
uint8_t comma[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x40}; 
uint8_t dash[8] = {0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00}; 
uint8_t underscore[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00}; 
uint8_t colon[8] = {0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00}; 
uint8_t semicolon[8] = {0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x20}; 
uint8_t quotationmark[8] = {0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
uint8_t space[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
// numbers
uint8_t one[8] = {0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00}; 
uint8_t two[8] = {0x38, 0x44, 0x08, 0x10, 0x20, 0x40, 0x7C, 0x00}; 
uint8_t three[8] = {0x38, 0x44, 0x04, 0x08, 0x04, 0x44, 0x38, 0x00}; 
uint8_t four[8] = {0x40, 0x48, 0x48, 0x7C, 0x08, 0x08, 0x08, 0x00}; 
uint8_t five[8] = {0x7C, 0x40, 0x78, 0x04, 0x04, 0x44, 0x38, 0x00}; 
uint8_t six[8] = {0x38, 0x44, 0x40, 0x78, 0x44, 0x44, 0x38, 0x00}; 
uint8_t seven[8] = {0x7C, 0x04, 0x08, 0x10, 0x20, 0x40, 0x40, 0x00}; 
uint8_t eight[8] = {0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38, 0x00}; 
uint8_t nine[8] = {0x38, 0x44, 0x44, 0x3C, 0x04, 0x44, 0x38, 0x00}; 
uint8_t zero[8] = {0x38, 0x44, 0x4C, 0x54, 0x54, 0x64, 0x38, 0x00}; 
uint8_t plus[8] = {0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x00}; 

// reset Matrix
for (uint8_t i=0; i<8; i++)
    MatrixCharacter[i] = 0;
// fill Matrix
for (uint8_t i=0; i<8; i++)
    switch (Character)
    {
	    case 'A':  MatrixCharacter[i] = A[i];  break;
	    case 'a':  MatrixCharacter[i] = A[i];  break;
	    case 'B':  MatrixCharacter[i] = B[i];  break;
	    case 'b':  MatrixCharacter[i] = B[i];  break;
	    case 'C':  MatrixCharacter[i] = C[i];  break;
	    case 'c':  MatrixCharacter[i] = C[i];  break;
	    case 'D':  MatrixCharacter[i] = D[i];  break;
	    case 'd':  MatrixCharacter[i] = D[i];  break;
	    case 'E':  MatrixCharacter[i] = E[i];  break;
	    case 'e':  MatrixCharacter[i] = E[i];  break;
	    case 'F':  MatrixCharacter[i] = F[i];  break;
	    case 'f':  MatrixCharacter[i] = F[i];  break;
	    case 'G':  MatrixCharacter[i] = G[i];  break;
	    case 'g':  MatrixCharacter[i] = G[i];  break;
	    case 'H':  MatrixCharacter[i] = H[i];  break;
	    case 'h':  MatrixCharacter[i] = H[i];  break;
	    case 'I':  MatrixCharacter[i] = I[i];  break;
	    case 'i':  MatrixCharacter[i] = I[i];  break;
	    case 'J':  MatrixCharacter[i] = J[i];  break;
	    case 'j':  MatrixCharacter[i] = J[i];  break;
	    case 'K':  MatrixCharacter[i] = K[i];  break;
	    case 'k':  MatrixCharacter[i] = K[i];  break;
	    case 'L':  MatrixCharacter[i] = L[i];  break;
	    case 'l':  MatrixCharacter[i] = L[i];  break;
	    case 'M':  MatrixCharacter[i] = M[i];  break;
	    case 'm':  MatrixCharacter[i] = M[i];  break;
	    case 'N':  MatrixCharacter[i] = N[i];  break;
	    case 'n':  MatrixCharacter[i] = N[i];  break;
	    case 'O':  MatrixCharacter[i] = O[i];  break;
	    case 'o':  MatrixCharacter[i] = O[i];  break;
	    case 'P':  MatrixCharacter[i] = P[i];  break;
	    case 'p':  MatrixCharacter[i] = P[i];  break;
	    case 'Q':  MatrixCharacter[i] = Q[i];  break;
	    case 'q':  MatrixCharacter[i] = Q[i];  break;
	    case 'R':  MatrixCharacter[i] = R[i];  break;
	    case 'r':  MatrixCharacter[i] = R[i];  break;
	    case 'S':  MatrixCharacter[i] = S[i];  break;
	    case 's':  MatrixCharacter[i] = S[i];  break;
	    case 'T':  MatrixCharacter[i] = T[i];  break;
	    case 't':  MatrixCharacter[i] = T[i];  break;
	    case 'U':  MatrixCharacter[i] = U[i];  break;
	    case 'u':  MatrixCharacter[i] = U[i];  break;
	    case 'V':  MatrixCharacter[i] = V[i];  break;
	    case 'v':  MatrixCharacter[i] = V[i];  break;
	    case 'W':  MatrixCharacter[i] = W[i];  break;
	    case 'w':  MatrixCharacter[i] = W[i];  break;
	    case 'X':  MatrixCharacter[i] = X[i];  break;
	    case 'x':  MatrixCharacter[i] = X[i];  break;
	    case 'Y':  MatrixCharacter[i] = Y[i];  break;
	    case 'y':  MatrixCharacter[i] = Y[i];  break;
	    case 'Z':  MatrixCharacter[i] = Z[i];  break;
	    case 'z':  MatrixCharacter[i] = Z[i];  break;
	    case ' ':  MatrixCharacter[i] = space[i];  break;
	    case '.':  MatrixCharacter[i] = dot[i];  break;
	    case ',':  MatrixCharacter[i] = comma[i];  break;
	    case '!':  MatrixCharacter[i] = exclamationmark[i];  break;
	    case '?':  MatrixCharacter[i] = questionmark[i];  break;
	    case '-':  MatrixCharacter[i] = dash[i];  break;
        case '_':  MatrixCharacter[i] = underscore[i];  break;
        case ':':  MatrixCharacter[i] = colon[i];  break;
        case ';':  MatrixCharacter[i] = semicolon[i];  break;
        case '"':  MatrixCharacter[i] = quotationmark[i];  break;
        case '+':  MatrixCharacter[i] = plus[i];  break;
        case '1':  MatrixCharacter[i] = one[i];  break;
        case '2':  MatrixCharacter[i] = two[i];  break;
        case '3':  MatrixCharacter[i] = three[i];  break;
        case '4':  MatrixCharacter[i] = four[i];  break;
        case '5':  MatrixCharacter[i] = five[i];  break;
        case '6':  MatrixCharacter[i] = six[i];  break;
        case '7':  MatrixCharacter[i] = seven[i];  break;
        case '8':  MatrixCharacter[i] = eight[i];  break;
        case '9':  MatrixCharacter[i] = nine[i];  break;
        case '0':  MatrixCharacter[i] = zero[i];  break;
	    default:  break;
    }
    return 0;
}


uint8_t scrolling_text(char *Text, const uint8_t ShowFreq, 
                       const uint8_t ShowLength, const uint8_t ShowWait, 
                       const uint8_t NumberHIGH, const uint8_t NumberLOW,
                       uint8_t pinHIGH[], uint8_t pinLOW[])
{
    // some variables
    uint8_t CharacterMatrix[8];     // matrix with letter
    uint8_t shortMatrix = 0;        // where are the LEDs
    uint8_t counterMatrix = 0;      // width of letter
    uint8_t LEDMatrix[8];           // matrix for show function
    // initialize matrix
    for (int a=0; a<8; a++)
        LEDMatrix[a] = 0;
    // get text length
    char *ptr = Text;
    uint8_t TextLength = 0;
    while (*ptr++)
        TextLength++;
    
    // animate scrolling text
    for (int i=0; i<TextLength; i++)
    {
        shortMatrix = 0;
        // get character matrix
        get_character(Text[i], CharacterMatrix);
        // where are the LEDs
        for (int a=0; a<8; a++)
            shortMatrix |= CharacterMatrix[a];
        // the width of the letter
        counterMatrix = 0;
        for (int a=0; a<8; a++)
            counterMatrix += (shortMatrix>>a & 1);
            
        // if space, add one column 
        if (counterMatrix == 0)
            for (int a=0; a<2; a++)
            {
                for (int b=0; b<8; b++)
                {
                    LEDMatrix[b] = (LEDMatrix[b] << 1);
                }
                showMatrix(LEDMatrix, ShowFreq, ShowLength, ShowWait, NumberHIGH, NumberLOW, pinHIGH, pinLOW);
            }
        // scroll the letter over the matrix
        for (int a=0; a<8; a++)
        {
            if (shortMatrix & (0x80>>a))
            {
                for (int b=0; b<8; b++)
                {
                    if (CharacterMatrix[b] & (0x80>>a))
                        LEDMatrix[b] = (LEDMatrix[b] << 1) | 1;
                    else
                        LEDMatrix[b] = (LEDMatrix[b] << 1);
                }
                showMatrix(LEDMatrix, ShowFreq, ShowLength, ShowWait, NumberHIGH, NumberLOW, pinHIGH, pinLOW);
            }
        }
        // add space between character
        for (int a=0; a<1; a++)
        {
            for (int b=0; b<8; b++)
            {
                LEDMatrix[b] = (LEDMatrix[b] << 1);
            }
            showMatrix(LEDMatrix, ShowFreq, ShowLength, ShowWait, NumberHIGH, NumberLOW, pinHIGH, pinLOW);
        }
    }
    
    // at the end of the text, empty matrix "scroll out"
    for (int a=0; a<8; a++)
    {
        for (int b=0; b<8; b++)
        {
            LEDMatrix[b] = (LEDMatrix[b] << 1);
        }
        showMatrix(LEDMatrix, ShowFreq, ShowLength, ShowWait, NumberHIGH, NumberLOW, pinHIGH, pinLOW);
    }

    return 0;
}



uint8_t showMatrix(uint8_t LEDmatrix[], const uint8_t ShowFreq,
                   const uint8_t ShowLength, const uint8_t ShowWait,
                   const uint8_t NumberHIGH, const uint8_t NumberLOW,
                   uint8_t pinHIGH[], uint8_t pinLOW[])
{
    // repeat the same picture "ShowLength times"
	for (int z=0; z<ShowLength; z++)
	{
		for (int i=0; i<NumberHIGH; i++)  // row
		{
            // set row LOW
			digitalWrite(pinLOW[i],0);
			for (int j=NumberLOW-1; j>-1; j--)  // column
			{
				byte bit = (LEDmatrix[j] >> i) & 1;
				if (bit) // LEDs turn on
				{
                    // set column HIGH
					digitalWrite(pinHIGH[j],1);
				}
			}
			delay(ShowFreq);  // row is on, for "ShowFreq" (ms)
			digitalWrite(pinLOW[i],1); // set low LEDs HIGH
			for (int j=0; j<8; j++) // set high LEDs LOW
			    digitalWrite(pinHIGH[j],0);
		}
		delay(ShowWait); // wait after all LEDs are off
	}
    return 0;
}



uint8_t animation(const uint8_t ShowFreq,
                  const uint8_t ShowLength,
                  const uint8_t ShowWait,
                  const uint8_t NumberHIGH, const uint8_t NumberLOW,
                  uint8_t pinHIGH[], uint8_t pinLOW[])
{
    byte sequence[] = { // a little animation
    B11111111,B10000001,B10000001,B10000001,B10000001,B10000001,B10000001,B11111111,
    B00000000,B01111110,B01000010,B01000010,B01000010,B01000010,B01111110,B00000000,
    B00000000,B00000000,B00111100,B00100100,B00100100,B00111100,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00011000,B00011000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00011000,B00011000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00111100,B00100100,B00100100,B00111100,B00000000,B00000000,
    B00000000,B01111110,B01000010,B01000010,B01000010,B01000010,B01111110,B00000000,
    B11111111,B10000001,B10000001,B10000001,B10000001,B10000001,B10000001,B11111111,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B01100110,B01100110,B00000000,B00000000,B01000010,B00111100,B00000000,
    B00000000,B01100110,B01100110,B00000000,B00000000,B01000010,B00111100,B00000000,
    B00000000,B00000000,B01100110,B00000000,B00000000,B01000010,B00111100,B00000000,
    B00000000,B00000000,B01100110,B00000000,B00000000,B01000010,B00111100,B00000000,
    B00000000,B01100110,B01100110,B00000000,B00000000,B01000010,B00111100,B00000000,
    B00000000,B01100110,B01100110,B00000000,B00000000,B01000010,B00111100,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
    };
    
    uint16_t length = sizeof(sequence);
    uint8_t LEDMatrix[8];           // matrix for show function
    
    int a = 0;
    for (int i=0; i<length; i++, a++)
    {
        LEDMatrix[a] = sequence[i];
        if (a==7)
        {
            showMatrix(LEDMatrix, ShowFreq, ShowLength, ShowWait, NumberHIGH, NumberLOW, pinHIGH, pinLOW);
            a = -1;
        }
    }
    for (int i=0; i<8; i++)
        LEDMatrix[i] = 0;
    
    return 0;
}









