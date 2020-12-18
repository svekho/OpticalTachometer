/* 
 * File:   lcd.h
 * Author: Sandra Ekholm
 * 
 * DESCRIPTION
 *      Header file for lcd module.
 *
 * Created on 16 December 2020, 17:33
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

    void lcd_init(void);
    void lcd_update(int rpm);


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

