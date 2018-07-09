#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#define DRIVER_NAME "ivshmem"
#define CDEV_NAME "ivshmem_cdev"
#define IVSHMEM_VENDOR_ID 0x1AF4
#define IVSHMEM_DEVICE_ID 0x1110
#define CMD_READ_SHMEM 0
#define CMD_FAKE1 1


static int event_irq = -1;
static int major_nr;


irqreturn_t irq_handler(int irq, void *dev_id)
{
  printk(KERN_INFO "irq_handler get called!, irq_number: %d", irq);
  return IRQ_HANDLED;
}

static int ivshmem_probe(struct pci_dev *dev, const struct pci_device_id *id)
{

  unsigned int bar_addr;
  int nvec;
  int ret;
  printk(KERN_DEBUG "Probe function get called\n");

  // print some info for experiments
  // print pci revision
  // using qemu version lower than 2.6 will read 0, otherwise 1
  printk(KERN_INFO "The device revision is %u\n", dev->revision);

  /*
  // print BAR0,1,2 addresses
  int ret = 0;
  unsigned int bar0, bar1, bar2;
  if (ret = pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &bar0))
    return ret;
  printk(KERN_INFO "BAR0: %08x", bar0);
  if (ret = pci_read_config_dword(dev, PCI_BASE_ADDRESS_1, &bar1))
    return ret;
  printk(KERN_INFO "BAR1: %08x", bar1);
  if (ret = pci_read_config_dword(dev, PCI_BASE_ADDRESS_2, &bar2))
    return ret;
  printk(KERN_INFO "BAR2: %08x", bar2);
  */

  // enable the PCI device
  if (pci_enable_device(dev))
    return -ENODEV;
  printk(KERN_DEBUG "Successfully enable the device\n");

  // request the region
  if (pci_request_regions(dev, DRIVER_NAME))
    goto out_disable;
  printk(KERN_DEBUG "Successfully reserve the resource\n");

  // access BAR address using pci_resource_start

  bar_addr = pci_resource_start(dev, 0);
  printk(KERN_INFO "BAR0: 0x%08x", bar_addr);
  bar_addr = pci_resource_start(dev, 2);
  printk(KERN_INFO "BAR2: 0x%08x", bar_addr);

  // play with MSI
  // allocate 1 interrupt vector
  nvec = pci_alloc_irq_vectors(dev, 1, 1, PCI_IRQ_MSIX);
  if (nvec < 0){
    printk(KERN_ERR "Fail to allocate irq vectors %d", nvec);
    goto out_release;
  }
  printk(KERN_DEBUG "Successfully allocate %d irq vectors", nvec);

  // get the irq number with the first vector (vector number = 0)
  event_irq = pci_irq_vector(dev, 0);
  printk(KERN_DEBUG "The irq number is %d", event_irq);

  ret = request_irq(event_irq, irq_handler, IRQF_SHARED, DRIVER_NAME, dev);
  if (ret) {
    printk(KERN_ERR "Fail to request shared irq, error: %d", ret);
    goto out_free_vec;
  }
  printk(KERN_DEBUG "Successfully request irq with number %d", event_irq);

  return 0;

out_free_vec:
  pci_free_irq_vectors(dev);
out_release:
  pci_release_regions(dev);
out_disable:
  pci_disable_device(dev);
  return -ENODEV;

}

static void ivshmem_remove(struct pci_dev *dev)
{
  if (event_irq != -1)
    free_irq(event_irq, dev);
  pci_free_irq_vectors(dev);
  pci_release_regions(dev);
  pci_disable_device(dev);
  printk(KERN_DEBUG "Remove function get called, resource freed\n");
  return;
}

 static struct pci_device_id ivshmem_pci_ids[] = {
 	{PCI_DEVICE(IVSHMEM_VENDOR_ID, IVSHMEM_DEVICE_ID)},
 	{ /* end: all zeroes */ }
 };


 static struct pci_driver ivshmem_pci_driver = {
    .name = DRIVER_NAME,
    .id_table = ivshmem_pci_ids,
    .probe = ivshmem_probe,
    .remove = ivshmem_remove,
};




static int fake_open(struct inode *i, struct file *f)
{
  printk(KERN_INFO "chardev file opened!");
  return 0;
}
static int fake_close(struct inode *i, struct file *f)
{
  printk(KERN_INFO "chardev file closed!");
  return 0;
}

static long ivshmem_ioctl(struct file *f, unsigned int cmd, unsigned long arg){
  switch (cmd){
    case CMD_READ_SHMEM:
      printk(KERN_INFO "IOCTL: read shared mem");
      break;
    case CMD_FAKE1:
      printk(KERN_INFO "IOCTL: fake command");
      break;

  }
  return 0;
}

static struct file_operations ivshmem_fops =
{
    .owner = THIS_MODULE,
    .open = fake_open,
    .release = fake_close,
    .unlocked_ioctl = ivshmem_ioctl
};

static int __init ivshmem_init_module(void)
{
	int ret;

  ret = register_chrdev(0, DRIVER_NAME, &ivshmem_fops);
	if (ret < 0) {
		printk(KERN_ERR "Unable to register ivshmem device\n");
		return ret;
  }
  major_nr = ret;
  printk("IVSHMEM: Major device number is: %d\n", major_nr);

  ret = pci_register_driver(&ivshmem_pci_driver);
  if (ret < 0) {
		goto error;
  }
	return 0;
error:
  	unregister_chrdev(major_nr, DRIVER_NAME);
  return ret;
}

static void __exit ivshmem_exit_module(void)
{
  pci_unregister_driver(&ivshmem_pci_driver);
  unregister_chrdev(major_nr, DRIVER_NAME);
  printk(KERN_DEBUG "Module exit");
}

MODULE_DEVICE_TABLE(pci, ivshmem_pci_ids);
MODULE_DESCRIPTION("A simple ivshmem PCI driver");
MODULE_AUTHOR("Gavin");
MODULE_LICENSE("GPL");
module_init(ivshmem_init_module);
module_exit(ivshmem_exit_module);
