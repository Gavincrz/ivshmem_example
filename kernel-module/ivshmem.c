#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>

#define DRIVER_NAME "ivshmem"
#define IVSHMEM_VENDOR_ID 0x1AF4
#define IVSHMEM_DEVICE_ID 0x1110


static int ivshmem_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
  printk(KERN_DEBUG "Probe function get called\n");
  // enable the PCI device
  if (pci_enable_device(dev))
    return -ENODEV;
  printk(KERN_DEBUG "Successfully enable the device\n");
  return 0;
}

static void ivshmem_remove(struct pci_dev *dev)
{
  printk(KERN_DEBUG "Remove function get called\n");
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


static int __init ivshmem_init_module(void)
{
	int ret;
  ret = pci_register_driver(&ivshmem_pci_driver);
  if (ret == 0)
  {
    printk(KERN_DEBUG "Module loaded\n");
  }
  else
  {
    printk(KERN_ERR "Module failed to loaded\n");
  }
	return ret;
}

static void __exit ivshmem_exit_module(void)
{
  pci_unregister_driver(&ivshmem_pci_driver);
  printk(KERN_DEBUG "Module exit");
}

MODULE_DEVICE_TABLE(pci, ivshmem_pci_ids);
MODULE_DESCRIPTION("A simple ivshmem PCI driver");
MODULE_AUTHOR("Gavin");
MODULE_LICENSE("GPL");
module_init(ivshmem_init_module);
module_exit(ivshmem_exit_module);
