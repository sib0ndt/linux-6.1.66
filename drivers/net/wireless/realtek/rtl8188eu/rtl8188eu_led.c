// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2016 Realtek Corporation. All rights reserved. */


#include <drv_types.h>
#include <rtl8188e_hal.h>

/* ********************************************************************************
 * LED object.
 * ******************************************************************************** */


/* ********************************************************************************
 *	Prototype of protected function.
 * ******************************************************************************** */


/* ********************************************************************************
 * LED_819xUsb routines.
 * ******************************************************************************** */

/*
 *	Description:
 *		Turn on LED according to LedPin specified.
 *   */
static void
SwLedOn_8188EU(
	_adapter			*padapter,
	PLED_USB		pLed
)
{
	u8	LedCfg;
	/* HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter); */

	if (RTW_CANNOT_RUN(padapter))
		return;

	LedCfg = rtw_read8(padapter, REG_LEDCFG2);
	switch (pLed->LedPin) {
	case LED_PIN_LED0:
		rtw_write8(padapter, REG_LEDCFG2, (LedCfg & 0xf0) | BIT5 | BIT6); /* SW control led0 on. */
		break;

	case LED_PIN_LED1:
		rtw_write8(padapter, REG_LEDCFG2, (LedCfg & 0x0f) | BIT5); /* SW control led1 on. */
		break;

	default:
		break;
	}

	pLed->bLedOn = true;
}


/*
 *	Description:
 *		Turn off LED according to LedPin specified.
 *   */
static void
SwLedOff_8188EU(
	_adapter			*padapter,
	PLED_USB		pLed
)
{
	u8	LedCfg;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	if (RTW_CANNOT_RUN(padapter))
		goto exit;


	LedCfg = rtw_read8(padapter, REG_LEDCFG2);/* 0x4E */

	switch (pLed->LedPin) {
	case LED_PIN_LED0:
		if (pHalData->bLedOpenDrain == true) { /* Open-drain arrangement for controlling the LED) */
			LedCfg &= 0x90; /* Set to software control.				 */
			rtw_write8(padapter, REG_LEDCFG2, (LedCfg | BIT3));
			LedCfg = rtw_read8(padapter, REG_MAC_PINMUX_CFG);
			LedCfg &= 0xFE;
			rtw_write8(padapter, REG_MAC_PINMUX_CFG, LedCfg);
		} else
			rtw_write8(padapter, REG_LEDCFG2, (LedCfg | BIT3 | BIT5 | BIT6));
		break;

	case LED_PIN_LED1:
		LedCfg &= 0x0f; /* Set to software control. */
		rtw_write8(padapter, REG_LEDCFG2, (LedCfg | BIT3));
		break;

	default:
		break;
	}
exit:
	pLed->bLedOn = false;

}

/* ********************************************************************************
 * Interface to manipulate LED objects.
 * ******************************************************************************** */


/* ********************************************************************************
 * Default LED behavior.
 * ******************************************************************************** */

/*
 *	Description:
 *		Initialize all LED_871x objects.
 *   */
void
rtl8188eu_InitSwLeds(
	_adapter	*padapter
)
{
	struct led_priv *pledpriv = &(padapter->ledpriv);

	pledpriv->LedControlHandler = LedControlUSB;

	pledpriv->SwLedOn = SwLedOn_8188EU;
	pledpriv->SwLedOff = SwLedOff_8188EU;

	InitLed(padapter, &(pledpriv->SwLed0), LED_PIN_LED0);

	InitLed(padapter, &(pledpriv->SwLed1), LED_PIN_LED1);
}


/*
 *	Description:
 *		DeInitialize all LED_819xUsb objects.
 *   */
void
rtl8188eu_DeInitSwLeds(
	_adapter	*padapter
)
{
	struct led_priv	*ledpriv = &(padapter->ledpriv);

	DeInitLed(&(ledpriv->SwLed0));
	DeInitLed(&(ledpriv->SwLed1));
}
