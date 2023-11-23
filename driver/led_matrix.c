#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/err.h>
#include <linux/types.h>

#include "font.h"

#define GPIO_LA 17
#define GPIO_CLK 27
#define GPIO_DI 22
#define GPIO_EN 23

#define RISING_EDGE(pin)        \
    do {                        \
        gpio_set_value(pin, 1); \
        udelay(10);              \
        gpio_set_value(pin, 0); \
    } while (0)                 \

dev_t dev = 0;

static struct class *dev_class;
static struct cdev cdev;

static char *led_buffer;
 
static void clock_edge(void)
{
    RISING_EDGE(GPIO_CLK);
}

static void latch_edge(void)
{
    RISING_EDGE(GPIO_LA);
}

static void push_bit(int bit)
{
    gpio_set_value(GPIO_DI, bit);
    clock_edge();
}

static int open(struct inode *inode, struct file *file)
{
  return 0;
}

static int release(struct inode *inode, struct file *file)
{
  return 0;
}
 
static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
  return 0;
}

static void init_gpio(int num, const char* label)
{
    gpio_request(num, label);
    gpio_direction_output(num, 0);
}

static void release_gpio(int num)
{
    gpio_unexport(num);
    gpio_free(num);
}

static void load_character(char c, int sx, int sy)
{
  char *letter = font8x8_basic[(int)c];

  for (int i = 0; i < 8; i++) {
    led_buffer[sx + sy * 16 + i * 2] = letter[i];
  }
}

static void display_buffer(void)
{
  // Mapping of row positions in modules
  int offset_mapping[] = { 1, 3, 0, 2, 4, 6, 5, 7 };
  // Iterate over all 16x4 modules
  for (int mod = 0; mod < 4; mod++) {
    // Iterate over all 8 8-led rows
    for (int i = 0; i < 8; i++) {
      // Extract matching byte from led buffer (starting top-left)
      char byte = led_buffer[mod * 8 + offset_mapping[i]];
      // Iterate over all leds in row
      for (int j = 0; j < 8; j++) {
        // Determine direction (left-to-right in even rows)
        int index = i % 2 ? j : 7 - j;
        // Extract bit
        char bit = (byte >> index) & 1;
        // Push to register
        push_bit(bit);
      }
    }
  }
  // Display!
  latch_edge();
}

static ssize_t write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
  if (len <= 5) {
    uint8_t rec_buf[4] = { 0, 0, 0, 0 };

    // This is safe since copy_from_user zero-padds data
    copy_from_user(rec_buf, buf, 4);

    for (int y = 0; y < 2; y++) {
      for (int x = 0; x < 2; x++) {
        load_character(rec_buf[2 * y + x], x, y);
      }
    }
  }

  // Display received data
  display_buffer();
  
  return len;
}

static struct file_operations fops =
{
  .owner = THIS_MODULE,
  .read = read,
  .write = write,
  .open = open,
  .release = release,
};

static int __init matrix_init(void)
{
  alloc_chrdev_region(&dev, 0, 1, "matrix_dev");
  cdev_init(&cdev, &fops);
  cdev_add(&cdev, dev, 1);
  dev_class = class_create(THIS_MODULE, "led_matrix_class");
  
  struct device *device = device_create(dev_class, NULL, dev, NULL, "led_matrix");

  led_buffer = devm_kzalloc(device, 32, GFP_KERNEL);

  init_gpio(GPIO_LA, "Matrix_Latch");
  init_gpio(GPIO_CLK, "Matrix_Clock");
  init_gpio(GPIO_DI, "Matrix_Data");
  init_gpio(GPIO_EN, "Matrix_Enable");

  display_buffer();

  return 0;
}

static void __exit matrix_exit(void)
{
  release_gpio(GPIO_LA);
  release_gpio(GPIO_CLK);
  release_gpio(GPIO_DI);
  release_gpio(GPIO_EN);

  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&cdev);
  unregister_chrdev_region(dev, 1);
}
 
module_init(matrix_init);
module_exit(matrix_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matthias Rupp <matthias.rupp@postoe.de>");
MODULE_DESCRIPTION("LED Matrix Driver");
MODULE_VERSION("1.00");