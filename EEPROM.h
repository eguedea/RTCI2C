/*
 * EEPROM.h
 *
 *  Created on: Mar 26, 2018
 *      Author: eric_
 */

#ifndef EEPROM_H_
#define EEPROM_H_

/*\brief funcion para escribir en memoria recibe una direccion de 16 bits y el dato que se quiere escribir, por medio del protocolo estandar de I2C
 * \param[add] direccion a la que se quiere escribir
 *\param[data] datos a escribir
 * */
void writeMemory(uint16_t add,uint8_t data);
/*\brief funcion para leer la memoria recibe una direccion de 16 bits para obtener el dato por medio del protocolo estandar de I2C
 * \param[add] direccion a la que se quiere leer
 * */
uint8_t readMemory(uint16_t add);


#endif /* EEPROM_H_ */
