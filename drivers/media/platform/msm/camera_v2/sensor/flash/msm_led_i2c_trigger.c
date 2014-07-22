/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define pr_fmt(fmt) "%s:%d " fmt, __func__, __LINE__

#include <mach/gpiomux.h>
#include <linux/module.h>
#include "msm_led_flash.h"
#include "msm_camera_io_util.h"
#include "../msm_sensor.h"
#include "msm_led_flash.h"
#include <linux/debugfs.h>

#define FLASH_NAME "camera-led-flash"

#define CONFIG_MSMB_CAMERA_DEBUG
//#define USE_GPIO

#undef CDBG
#ifdef CONFIG_MSMB_CAMERA_DEBUG
#define CDBG(fmt, args...) pr_err(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#endif

/* LGE_CHANGE_S, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/
unsigned char strobe_ctrl;
unsigned char  flash_ctrl;

/* LGE_CHANGE_S, yt.jeon@lge.com, To fix an issue of flash widget 2013-10-30*/
extern void rt8542_led_enable(void);
extern void rt8542_led_disable(void);
/* LGE_CHANGE_E, yt.jeon@lge.com, To fix an issue of flash widget 2013-10-30*/

/*                                                           */
unsigned char strobe_ctrl;
unsigned char  flash_ctrl;

/*                                                                          */
extern void rt8542_led_enable(void);
extern void rt8542_led_disable(void);
/*                                                                          */

static int flash_read_reg(struct msm_camera_i2c_client *client, unsigned char reg, unsigned char *data)
{
	int err;
	struct i2c_msg msg[] = {
		{
			client->client->addr, 0, 1, &reg
		},
		{
			client->client->addr, I2C_M_RD, 1, data
		},
	};

	err = i2c_transfer(client->client->adapter, msg, 2);
	if (err < 0) {
		pr_err("i2c read error\n");
		return err;
	}
	return 0;
}
static int flash_write_reg(struct msm_camera_i2c_client *client, unsigned char reg, unsigned char data)
{
	int err;
	unsigned char buf[2];
	struct i2c_msg msg = {
		client->client->addr, 0, 2, buf
	};

	buf[0] = reg;
	buf[1] = data;

	err = i2c_transfer(client->client->adapter, &msg, 1);
	if (err < 0) {
		pr_err("i2c write error\n");
	}
	return 0;
}

/* LGE_CHANGE_E, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                           */


int32_t msm_led_i2c_trigger_get_subdev_id(struct msm_led_flash_ctrl_t *fctrl,
	void *arg)
{
	uint32_t *subdev_id = (uint32_t *)arg;
	if (!subdev_id) {
		pr_err("failed\n");
		return -EINVAL;
	}
	*subdev_id = fctrl->subdev_id;

	CDBG("subdev_id %d\n", *subdev_id);
	return 0;
}

int32_t msm_led_i2c_trigger_config(struct msm_led_flash_ctrl_t *fctrl,
	void *data)
{
	int rc = 0;
	struct msm_camera_led_cfg_t *cfg = (struct msm_camera_led_cfg_t *)data;
	CDBG("called led_state %d\n", cfg->cfgtype);

	if (!fctrl->func_tbl) {
		pr_err("failed\n");
		return -EINVAL;
	}
	switch (cfg->cfgtype) {

	case MSM_CAMERA_LED_INIT:
		if (fctrl->func_tbl->flash_led_init)
			rc = fctrl->func_tbl->flash_led_init(fctrl);
		break;

	case MSM_CAMERA_LED_RELEASE:

		rt8542_led_disable();/* LGE_CHANGE, yt.jeon@lge.com, To fix an issue of flash widget 2013-10-30*/

		rt8542_led_disable();/*                                                                        */

		if (fctrl->func_tbl->flash_led_release)
			rc = fctrl->func_tbl->
				flash_led_release(fctrl);
		break;

	case MSM_CAMERA_LED_OFF:

		rt8542_led_disable();/* LGE_CHANGE, yt.jeon@lge.com, To fix an issue of flash widget 2013-10-30*/

		rt8542_led_disable();/*                                                                        */

		if (fctrl->func_tbl->flash_led_off)
			rc = fctrl->func_tbl->flash_led_off(fctrl);
		break;

	case MSM_CAMERA_LED_LOW:

		rt8542_led_enable();/* LGE_CHANGE, yt.jeon@lge.com, To fix an issue of flash widget 2013-10-30*/

		rt8542_led_enable();/*                                                                        */

		if (fctrl->func_tbl->flash_led_low)
			rc = fctrl->func_tbl->flash_led_low(fctrl);
		break;

	case MSM_CAMERA_LED_HIGH:

		rt8542_led_enable();/* LGE_CHANGE, yt.jeon@lge.com, To fix an issue of flash widget 2013-10-30*/

		rt8542_led_enable();/*                                                                        */

		if (fctrl->func_tbl->flash_led_high)
			rc = fctrl->func_tbl->flash_led_high(fctrl);
		break;
	default:
		rc = -EFAULT;
		break;
	}
	CDBG("flash_set_led_state: return %d\n", rc);
	return rc;
}

int msm_flash_led_init(struct msm_led_flash_ctrl_t *fctrl)
{
	int rc = 0;

/* LGE_CHANGE_S, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/

/*                                                                               */

	CDBG("%s:%d called\n", __func__, __LINE__);

#ifdef USE_GPIO
	struct msm_camera_sensor_board_info *flashdata = NULL;

	flashdata = fctrl->flashdata;
	if (flashdata->gpio_conf->cam_gpiomux_conf_tbl != NULL) {
		pr_err("%s:%d mux install\n", __func__, __LINE__);
		msm_gpiomux_install(
			(struct msm_gpiomux_config *)
			flashdata->gpio_conf->cam_gpiomux_conf_tbl,
			flashdata->gpio_conf->cam_gpiomux_conf_tbl_size);
	}

	rc = msm_camera_request_gpio_table(
		flashdata->gpio_conf->cam_gpio_req_tbl,
		flashdata->gpio_conf->cam_gpio_req_tbl_size, 1);
	if (rc < 0) {
		pr_err("%s: request gpio failed\n", __func__);
		return rc;
	}
	msleep(20);
	gpio_set_value_cansleep(
		flashdata->gpio_conf->gpio_num_info->gpio_num[0],
		GPIO_OUT_HIGH);
#endif

/* LGE_CHANGE_E, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/

#if 0 /* LGE_CHANGE, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                                               */

#if 0 /*                                                         */

	if (fctrl->flash_i2c_client && fctrl->reg_setting) {
		rc = fctrl->flash_i2c_client->i2c_func_tbl->i2c_write_table(
			fctrl->flash_i2c_client,
			fctrl->reg_setting->init_setting);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
#endif
	return rc;
}

int msm_flash_led_release(struct msm_led_flash_ctrl_t *fctrl)
{
	int rc = 0;

/* LGE_CHANGE_S, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/

/*                                                                               */

	CDBG("%s:%d called\n", __func__, __LINE__);

#ifdef USE_GPIO
	struct msm_camera_sensor_board_info *flashdata = NULL;

	flashdata = fctrl->flashdata;

	if (!fctrl) {
		pr_err("%s:%d fctrl NULL\n", __func__, __LINE__);
		return -EINVAL;
	}
	gpio_set_value_cansleep(
		flashdata->gpio_conf->gpio_num_info->gpio_num[0],
		GPIO_OUT_LOW);

/* LGE_CHANGE_S, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/
	//gpio_set_value_cansleep(
	//	flashdata->gpio_conf->gpio_num_info->gpio_num[1],
	//	GPIO_OUT_LOW);
/* LGE_CHANGE_E, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                           */
	//gpio_set_value_cansleep(
	//	flashdata->gpio_conf->gpio_num_info->gpio_num[1],
	//	GPIO_OUT_LOW);
/*                                                           */

	rc = msm_camera_request_gpio_table(
		flashdata->gpio_conf->cam_gpio_req_tbl,
		flashdata->gpio_conf->cam_gpio_req_tbl_size, 0);
	if (rc < 0) {
		pr_err("%s: request gpio failed\n", __func__);
		return rc;
	}
#else
	if (fctrl->flash_i2c_client) {
			rc =flash_write_reg(fctrl->flash_i2c_client, 0x0A, 0x1D); //clear bit1,5,6
			if (rc < 0)
				pr_err("%s:%d failed\n", __func__, __LINE__);

	}
#endif
/* LGE_CHANGE_E, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/

#ifdef CONFIG_MACH_MSM8X10_W5
#ifndef CONFIG_MACH_MSM8X10_W55   /* Disable only w55 device for camera flash issue, hanyoung.kim 140114 */
			rc =flash_write_reg(fctrl->flash_i2c_client, 0x09, 0x5D);
			if (rc < 0)
				pr_err("%s:%d failed\n", __func__, __LINE__);
#endif
#endif
	}
#endif
/*                                                                               */


	return 0;
}

int msm_flash_led_off(struct msm_led_flash_ctrl_t *fctrl)
{
	int rc = 0;

/* LGE_CHANGE_S, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/

/*                                                                               */

	CDBG("%s:%d called\n", __func__, __LINE__);
#ifdef USE_GPIO
	struct msm_camera_sensor_board_info *flashdata = NULL;

	flashdata = fctrl->flashdata;

	if (!fctrl) {
		pr_err("%s:%d fctrl NULL\n", __func__, __LINE__);
		return -EINVAL;
	}
#endif

/* LGE_CHANGE_E, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/

#if 0/* LGE_CHANGE, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                                               */

#if 0/*                                                         */

	if (fctrl->flash_i2c_client && fctrl->reg_setting) {
		rc = fctrl->flash_i2c_client->i2c_func_tbl->i2c_write_table(
			fctrl->flash_i2c_client,
			fctrl->reg_setting->off_setting);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
#endif

/* LGE_CHANGE_S, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                           */

#ifdef USE_GPIO
	gpio_set_value_cansleep(
		flashdata->gpio_conf->gpio_num_info->gpio_num[0],
		GPIO_OUT_LOW);
#else

/* LGE_CHANGE_S, WBT issue fix, 2013-11-25, hyunuk.park@lge.com */

/*                                                              */

    if(fctrl->flash_i2c_client == NULL) {
        pr_err("%s:%d fctrl->flash_i2c_client NULL\n", __func__, __LINE__);
		return -EINVAL;
    }

/* LGE_CHANGE_E, WBT issue fix, 2013-11-25, hyunuk.park@lge.com */

/*                                                              */

	if (fctrl->flash_i2c_client) {
			rc =flash_write_reg(fctrl->flash_i2c_client, 0x0A, 0x1D); //clear bit1,5,6
			if (rc < 0)
				pr_err("%s:%d failed\n", __func__, __LINE__);


#ifdef CONFIG_MACH_MSM8X10_W5
#ifndef CONFIG_MACH_MSM8X10_W55    /* Disable only w55 device for camera flash issue, hanyoung.kim 140114 */
			rc =flash_write_reg(fctrl->flash_i2c_client, 0x09, 0x5D);
			if (rc < 0)
				pr_err("%s:%d failed\n", __func__, __LINE__);
#endif
#endif

	}
	//gpio_set_value_cansleep(
	//	flashdata->gpio_conf->gpio_num_info->gpio_num[1],
	//	GPIO_OUT_LOW);
#endif

/* LGE_CHANGE_E, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                           */


	return rc;
}

int msm_flash_led_low(struct msm_led_flash_ctrl_t *fctrl)
{
	int rc = 0;

/* LGE_CHANGE_S, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/
	CDBG("%s:%d called\n", __func__, __LINE__);

/* LGE_CHANGE_S, WBT issue fix, 2013-11-25, hyunuk.park@lge.com */

/*                                                                               */
	CDBG("%s:%d called\n", __func__, __LINE__);

/*                                                              */

    if(fctrl == NULL) {
        pr_err("%s:%d fctrl\n", __func__, __LINE__);
		return -EINVAL;
    }

    if(fctrl->flash_i2c_client == NULL) {
        pr_err("%s:%d fctrl->flash_i2c_client NULL\n", __func__, __LINE__);
		return -EINVAL;
    }

/* LGE_CHANGE_E, WBT issue fix, 2013-11-25, hyunuk.park@lge.com */

/*                                                              */


#ifdef USE_GPIO
	struct msm_camera_sensor_board_info *flashdata = NULL;
#endif

/* LGE_CHANGE_E, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/

/* LGE_CHANGE_S, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                                               */

/*                                                           */

#if 0
	//gpio_set_value_cansleep(
	//	flashdata->gpio_conf->gpio_num_info->gpio_num[1],
	//	GPIO_OUT_HIGH);


	if (fctrl->flash_i2c_client && fctrl->reg_setting) {
		rc = fctrl->flash_i2c_client->i2c_func_tbl->i2c_write_table(
			fctrl->flash_i2c_client,
			fctrl->reg_setting->low_setting);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
#else
	if (fctrl->flash_i2c_client) {
		/* Configuration of frequency, current limit and timeout, default 2Mhz, 1.7A, 1024ms */
		rc =flash_write_reg(fctrl->flash_i2c_client,	0x07, 0x0F);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
	if (fctrl->flash_i2c_client) {
		/* Configuration of current, torch : 112.5mA, strobe : 93.75mA */
		rc =flash_write_reg(fctrl->flash_i2c_client,	0x06, 0x31);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
#endif

#ifdef USE_GPIO
	flashdata = fctrl->flashdata;
	gpio_set_value_cansleep(
		flashdata->gpio_conf->gpio_num_info->gpio_num[0],
		GPIO_OUT_HIGH);
#else
	/* Control of I/O register */
	if (fctrl->flash_i2c_client) {
			rc =flash_write_reg(fctrl->flash_i2c_client, 0x0A, 0x1D); //clear bit1,5,6
			if (rc < 0)
				pr_err("%s:%d failed\n", __func__, __LINE__);
		}

	strobe_ctrl=0;
	rc = flash_read_reg(fctrl->flash_i2c_client, 0x09, &strobe_ctrl);
	if (rc < 0) {
		pr_err("%s:%d failed\n", __func__, __LINE__);
	}

	strobe_ctrl &= 0xDF; /* 1101 1111 */
	strobe_ctrl |= 0x10; /* 1010 1101 */
	if (fctrl->flash_i2c_client) {
		rc =flash_write_reg(fctrl->flash_i2c_client,	0x09, strobe_ctrl);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}

	/* Enable */
	flash_ctrl=0;
	rc = flash_read_reg(fctrl->flash_i2c_client, 0x0A, &flash_ctrl);
	if (rc < 0) {
		pr_err("%s:%d failed\n", __func__, __LINE__);
	}
	flash_ctrl &= 0xFB; /* 1111 1011 */
	flash_ctrl |= 0x62; /* 0100 0010 */

	//jongho3.lee@lge.com[2013-06-05] : Since default is FLASH(0x04) mode after rt8542 power on,

	//                                                                                          

	// we must set the third bit 0 to set TORCH mode.
	if (fctrl->flash_i2c_client) {
		rc =flash_write_reg(fctrl->flash_i2c_client, 0x0A, flash_ctrl);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
#endif

/* LGE_CHANGE_E, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                           */

	return rc;
}

int msm_flash_led_high(struct msm_led_flash_ctrl_t *fctrl)
{
	int rc = 0;

/* LGE_CHANGE_S, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/

/*                                                                               */

	CDBG("%s:%d called\n", __func__, __LINE__);
#ifdef USE_GPIO
	struct msm_camera_sensor_board_info *flashdata = NULL;
#endif

/* LGE_CHANGE_E, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/
   /* LGE_CHANGE_S, WBT issue fix, 2013-11-25, hyunuk.park@lge.com */

/*                                                                               */
   /*                                                              */

    if(fctrl == NULL) {
        pr_err("%s:%d fctrl\n", __func__, __LINE__);
		return -EINVAL;
    }

    if(fctrl->flash_i2c_client == NULL) {
        pr_err("%s:%d fctrl->flash_i2c_client NULL\n", __func__, __LINE__);
		return -EINVAL;
    }

/* LGE_CHANGE_E, WBT issue fix, 2013-11-25, hyunuk.park@lge.com */
/* LGE_CHANGE_S, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                              */
/*                                                           */

#if 0
	//gpio_set_value_cansleep(
	//	flashdata->gpio_conf->gpio_num_info->gpio_num[1],
	//	GPIO_OUT_HIGH);

	if (fctrl->flash_i2c_client && fctrl->reg_setting) {
		rc = fctrl->flash_i2c_client->i2c_func_tbl->i2c_write_table(
			fctrl->flash_i2c_client,
			fctrl->reg_setting->high_setting);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
#else
	if (fctrl->flash_i2c_client) {
		/* Configuration of frequency, current limit and timeout, default 2Mhz, 2.5A, 1024ms */
		rc =flash_write_reg(fctrl->flash_i2c_client,	0x07, 0x1F);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
	if (fctrl->flash_i2c_client) {
		/* Configuration of current, torch : 56.25mA, strobe : 384.375mA */
		rc =flash_write_reg(fctrl->flash_i2c_client,	0x06, 0x17);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
#endif


/* LGE_CHANGE_S, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/

/*                                                                               */

#ifdef USE_GPIO
	flashdata = fctrl->flashdata;
	gpio_set_value_cansleep(
		flashdata->gpio_conf->gpio_num_info->gpio_num[0],
		GPIO_OUT_HIGH);
#else
	/* Control of I/O register */
	if (fctrl->flash_i2c_client) {
			rc =flash_write_reg(fctrl->flash_i2c_client, 0x0A, 0x1D);
			if (rc < 0)
				pr_err("%s:%d failed\n", __func__, __LINE__);
		}

	strobe_ctrl=0;
	rc = flash_read_reg(fctrl->flash_i2c_client, 0x09, &strobe_ctrl);
	if (rc < 0) {
		pr_err("%s:%d failed\n", __func__, __LINE__);
	}

	strobe_ctrl &= 0xDF; /* 1101 1111 */
	strobe_ctrl |= 0x10; /* 1010 1101 */
	if (fctrl->flash_i2c_client) {
		rc =flash_write_reg(fctrl->flash_i2c_client,	0x09, strobe_ctrl);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}

	/* Enable */
	flash_ctrl=0;
	rc = flash_read_reg(fctrl->flash_i2c_client, 0x0A,	&flash_ctrl);
	if (rc < 0) {
		pr_err("%s:%d failed\n", __func__, __LINE__);
	}
	flash_ctrl &= 0xFD; /* clear bit1*/
	flash_ctrl |= 0x04; /* set bit2*/
	if (fctrl->flash_i2c_client) {
		rc =flash_write_reg(fctrl->flash_i2c_client, 0x0A,	flash_ctrl);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
	flash_ctrl |= 0x66; /* 0110 0110*/
	if (fctrl->flash_i2c_client) {
		rc =flash_write_reg(fctrl->flash_i2c_client, 0x0A,	flash_ctrl);
		if (rc < 0)
			pr_err("%s:%d failed\n", __func__, __LINE__);
	}
#endif

/* LGE_CHANGE_E, yt.jeon@lge.com, refine flash driver and enable torch 2013-10-22*/
/* LGE_CHANGE_E, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

/*                                                                               */
/*                                                           */

	return rc;
}

static int32_t msm_flash_init_gpio_pin_tbl(struct device_node *of_node,
	struct msm_camera_gpio_conf *gconf, uint16_t *gpio_array,
	uint16_t gpio_array_size)
{
	int32_t rc = 0;
	int32_t val = 0;

	gconf->gpio_num_info = kzalloc(sizeof(struct msm_camera_gpio_num_info),
		GFP_KERNEL);
	if (!gconf->gpio_num_info) {
		pr_err("%s failed %d\n", __func__, __LINE__);
		rc = -ENOMEM;
		return rc;
	}

	rc = of_property_read_u32(of_node, "qcom,gpio-flash-en", &val);
	if (rc < 0) {
		pr_err("%s:%d read qcom,gpio-flash-en failed rc %d\n",
			__func__, __LINE__, rc);
		goto ERROR;
	} else if (val >= gpio_array_size) {
		pr_err("%s:%d qcom,gpio-flash-en invalid %d\n",
			__func__, __LINE__, val);
		goto ERROR;
	}
	/*index 0 is for qcom,gpio-flash-en */
	gconf->gpio_num_info->gpio_num[0] =
		gpio_array[val];
	CDBG("%s qcom,gpio-flash-en %d\n", __func__,
		gconf->gpio_num_info->gpio_num[0]);

#if 0/* LGE_CHANGE, yt.jeon@lge.com, Flash driver B/U 2013-10-04*/

#if 0/*                                                         */

	rc = of_property_read_u32(of_node, "qcom,gpio-flash-now", &val);
	if (rc < 0) {
		pr_err("%s:%d read qcom,gpio-flash-now failed rc %d\n",
			__func__, __LINE__, rc);
		goto ERROR;
	} else if (val >= gpio_array_size) {
		pr_err("%s:%d qcom,gpio-flash-now invalid %d\n",
			__func__, __LINE__, val);
		goto ERROR;
	}
	/*index 1 is for qcom,gpio-flash-now */
	gconf->gpio_num_info->gpio_num[1] =
		gpio_array[val];
	CDBG("%s qcom,gpio-flash-now %d\n", __func__,
		gconf->gpio_num_info->gpio_num[1]);
#endif
	return rc;

ERROR:
	kfree(gconf->gpio_num_info);
	gconf->gpio_num_info = NULL;
	return rc;
}

static int32_t msm_led_get_dt_data(struct device_node *of_node,
		struct msm_led_flash_ctrl_t *fctrl)
{
	int32_t rc = 0, i = 0;
	struct msm_camera_gpio_conf *gconf = NULL;
	struct device_node *flash_src_node = NULL;
	struct msm_camera_sensor_board_info *flashdata = NULL;
	uint32_t count = 0;
	uint16_t *gpio_array = NULL;
	uint16_t gpio_array_size = 0;
	uint32_t id_info[3];

	CDBG("called\n");

	if (!of_node) {
		pr_err("of_node NULL\n");
		return -EINVAL;
	}

	fctrl->flashdata = kzalloc(sizeof(
		struct msm_camera_sensor_board_info),
		GFP_KERNEL);
	if (!fctrl->flashdata) {
		pr_err("%s failed %d\n", __func__, __LINE__);
		return -ENOMEM;
	}

	flashdata = fctrl->flashdata;

	flashdata->sensor_init_params = kzalloc(sizeof(
		struct msm_sensor_init_params), GFP_KERNEL);
	if (!flashdata->sensor_init_params) {
		pr_err("%s failed %d\n", __func__, __LINE__);
		return -ENOMEM;
	}

	rc = of_property_read_u32(of_node, "cell-index", &fctrl->subdev_id);
	if (rc < 0) {
		pr_err("failed\n");
		return -EINVAL;
	}

	CDBG("subdev id %d\n", fctrl->subdev_id);

	rc = of_property_read_string(of_node, "qcom,flash-name",
		&flashdata->sensor_name);
	CDBG("%s qcom,flash-name %s, rc %d\n", __func__,
		flashdata->sensor_name, rc);
	if (rc < 0) {
		pr_err("%s failed %d\n", __func__, __LINE__);
		goto ERROR1;
	}

	if (of_get_property(of_node, "qcom,flash-source", &count)) {
		count /= sizeof(uint32_t);
		CDBG("count %d\n", count);
		if (count > MAX_LED_TRIGGERS) {
			pr_err("failed\n");
			return -EINVAL;
		}
		for (i = 0; i < count; i++) {
			flash_src_node = of_parse_phandle(of_node,
				"qcom,flash-source", i);
			if (!flash_src_node) {
				pr_err("flash_src_node NULL\n");
				continue;
			}

			rc = of_property_read_string(flash_src_node,
				"linux,default-trigger",
				&fctrl->flash_trigger_name[i]);
			if (rc < 0) {
				pr_err("failed\n");
				of_node_put(flash_src_node);
				continue;
			}

			CDBG("default trigger %s\n",
				 fctrl->flash_trigger_name[i]);

			rc = of_property_read_u32(flash_src_node,
				"qcom,max-current",
				&fctrl->flash_op_current[i]);
			if (rc < 0) {
				pr_err("failed rc %d\n", rc);
				of_node_put(flash_src_node);
				continue;
			}

			of_node_put(flash_src_node);

			CDBG("max_current[%d] %d\n",
				i, fctrl->flash_op_current[i]);

			led_trigger_register_simple(
				fctrl->flash_trigger_name[i],
				&fctrl->flash_trigger[i]);
		}

	} else { /*Handle LED Flash Ctrl by GPIO*/
		flashdata->gpio_conf =
			 kzalloc(sizeof(struct msm_camera_gpio_conf),
				 GFP_KERNEL);
		if (!flashdata->gpio_conf) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			rc = -ENOMEM;
			return rc;
		}
		gconf = flashdata->gpio_conf;

		gpio_array_size = of_gpio_count(of_node);
		CDBG("%s gpio count %d\n", __func__, gpio_array_size);

		if (gpio_array_size) {
			gpio_array = kzalloc(sizeof(uint16_t) * gpio_array_size,
				GFP_KERNEL);
			if (!gpio_array) {
				pr_err("%s failed %d\n", __func__, __LINE__);
				rc = -ENOMEM;
				goto ERROR4;
			}
			for (i = 0; i < gpio_array_size; i++) {
				gpio_array[i] = of_get_gpio(of_node, i);
				CDBG("%s gpio_array[%d] = %d\n", __func__, i,
					gpio_array[i]);
			}

			rc = msm_sensor_get_dt_gpio_req_tbl(of_node, gconf,
				gpio_array, gpio_array_size);
			if (rc < 0) {
				pr_err("%s failed %d\n", __func__, __LINE__);
				goto ERROR4;
			}

			rc = msm_sensor_get_dt_gpio_set_tbl(of_node, gconf,
				gpio_array, gpio_array_size);
			if (rc < 0) {
				pr_err("%s failed %d\n", __func__, __LINE__);
				goto ERROR5;
			}

			rc = msm_flash_init_gpio_pin_tbl(of_node, gconf,
				gpio_array, gpio_array_size);
			if (rc < 0) {
				pr_err("%s failed %d\n", __func__, __LINE__);
				goto ERROR6;
			}
		}

		flashdata->slave_info =
			kzalloc(sizeof(struct msm_camera_slave_info),
				GFP_KERNEL);
		if (!flashdata->slave_info) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			rc = -ENOMEM;
			goto ERROR8;
		}

		rc = of_property_read_u32_array(of_node, "qcom,slave-id",
			id_info, 3);
		if (rc < 0) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			goto ERROR9;
		}
		fctrl->flashdata->slave_info->sensor_slave_addr = id_info[0];
		fctrl->flashdata->slave_info->sensor_id_reg_addr = id_info[1];
		fctrl->flashdata->slave_info->sensor_id = id_info[2];

		kfree(gpio_array);
		return rc;
ERROR9:
		kfree(fctrl->flashdata->slave_info);
ERROR8:
		kfree(fctrl->flashdata->gpio_conf->gpio_num_info);
ERROR6:
		kfree(gconf->cam_gpio_set_tbl);
ERROR5:
		kfree(gconf->cam_gpio_req_tbl);
ERROR4:
		kfree(gconf);
ERROR1:
		kfree(fctrl->flashdata);
		kfree(gpio_array);
	}
	return rc;
}

static struct msm_camera_i2c_fn_t msm_sensor_qup_func_tbl = {
	.i2c_read = msm_camera_qup_i2c_read,
	.i2c_read_seq = msm_camera_qup_i2c_read_seq,
	.i2c_write = msm_camera_qup_i2c_write,
	.i2c_write_table = msm_camera_qup_i2c_write_table,
	.i2c_write_seq_table = msm_camera_qup_i2c_write_seq_table,
	.i2c_write_table_w_microdelay =
		msm_camera_qup_i2c_write_table_w_microdelay,
};

#ifdef CONFIG_DEBUG_FS
static int set_led_status(void *data, u64 val)
{
	struct msm_led_flash_ctrl_t *fctrl =
		 (struct msm_led_flash_ctrl_t *)data;
	int rc = -1;
	pr_debug("set_led_status: Enter val: %llu", val);
	if (!fctrl) {
		pr_err("set_led_status: fctrl is NULL");
		return rc;
	}
	if (!fctrl->func_tbl) {
		pr_err("set_led_status: fctrl->func_tbl is NULL");
		return rc;
	}
	if (val == 0) {
		pr_debug("set_led_status: val is disable");
		rc = msm_flash_led_off(fctrl);
	} else {
		pr_debug("set_led_status: val is enable");
		rc = msm_flash_led_low(fctrl);
	}

	return rc;
}

DEFINE_SIMPLE_ATTRIBUTE(ledflashdbg_fops,
	NULL, set_led_status, "%llu\n");
#endif

int msm_flash_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_led_flash_ctrl_t *fctrl = NULL;
#ifdef CONFIG_DEBUG_FS
	struct dentry *dentry;
#endif
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("i2c_check_functionality failed\n");
		goto probe_failure;
	}

	fctrl = (struct msm_led_flash_ctrl_t *)(id->driver_data);

	/* LGE_CHANGE_S, WBT issue fix, 2013-11-25, hyunuk.park@lge.com */

	/*                                                              */

    if(fctrl == NULL) {
        pr_err("%s:%d fctrl\n", __func__, __LINE__);
		goto probe_failure;
    }

    if(fctrl->flash_i2c_client == NULL) {
        pr_err("%s:%d fctrl->flash_i2c_client NULL\n", __func__, __LINE__);
		goto probe_failure;
    }

    /* LGE_CHANGE_E, WBT issue fix, 2013-11-25, hyunuk.park@lge.com */

    /*                                                              */

	if (fctrl->flash_i2c_client)
		fctrl->flash_i2c_client->client = client;
	/* Set device type as I2C */
	fctrl->flash_device_type = MSM_CAMERA_I2C_DEVICE;

	/* Assign name for sub device */
	snprintf(fctrl->msm_sd.sd.name, sizeof(fctrl->msm_sd.sd.name),
		"%s", id->name);

	rc = msm_led_get_dt_data(client->dev.of_node, fctrl);
	if (rc < 0) {
		pr_err("%s failed line %d\n", __func__, __LINE__);
		return rc;
	}
	if (fctrl->flash_i2c_client != NULL) {
		fctrl->flash_i2c_client->client = client;
		if (fctrl->flashdata->slave_info->sensor_slave_addr)
			fctrl->flash_i2c_client->client->addr =
				fctrl->flashdata->slave_info->
				sensor_slave_addr;
	} else {
		pr_err("%s %s sensor_i2c_client NULL\n",
			__func__, client->name);
		rc = -EFAULT;
		return rc;
	}

	if (!fctrl->flash_i2c_client->i2c_func_tbl)
		fctrl->flash_i2c_client->i2c_func_tbl =
			&msm_sensor_qup_func_tbl;

	rc = msm_led_i2c_flash_create_v4lsubdev(fctrl);
#ifdef CONFIG_DEBUG_FS
	dentry = debugfs_create_file("ledflash", S_IRUGO, NULL, (void *)fctrl,
		&ledflashdbg_fops);
	if (!dentry)
		pr_err("Failed to create the debugfs ledflash file");
#endif
	CDBG("%s:%d probe success\n", __func__, __LINE__);
	return 0;

probe_failure:
	CDBG("%s:%d probe failed\n", __func__, __LINE__);
	return rc;
}
