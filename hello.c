/**
 * @file: hello.c
 * @author: samar lassoued
 *
 * This file contains the implementation of  a simple Linux Kernel module
 */

#include <linux/init.h>    // Macros for module initialization
#include <linux/module.h>  // Core header for loading modules
#include <linux/kernel.h>  // Kernel logging macros


/**
 * This function is called when the kernel module is loaded
 * into the kernel (via insmod). It performs any setup required
 * by the module.
 *
 * @param[in] void The number of words to read.
 * @return Returns 0 on success, negative error code on failure,
 * return an array of char* containing the words.
 * 
 * Notes:
 * - The function must match the prototype `int init(void)` for
 *   use with the module_init() macro.
 */
static int __init hello_init(void)
{
    printk(KERN_INFO "Hello, world!\n");
    return 0;  
}

/**
 * This function is called when the kernel module is removed
 * from the kernel (via rmmod). It performs any cleanup required
 * by the module.
 *  - The `__exit` macro indicates that this function is used
 *   only during module unload. On some architectures, the memory
 *   for this function can be discarded if the module is built-in
 *   (not loadable).
 * 
 * Notes:
 * - The function must match the prototype `int init(void)` for
 *   use with the module_init() macro.
 */
 
static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye, world!\n");
}

// Macros to define the module’s init and exit points
module_init(hello_init);
module_exit(hello_exit);



MODULE_LICENSE("GPL");               
MODULE_AUTHOR("Samar Lassoued");          
MODULE_DESCRIPTION("A simple module"); 
MODULE_VERSION("1.0");               