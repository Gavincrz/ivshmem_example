#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>

#define DRIVER_NAME "ivshmem"
#define IVSHMEM_VENDOR_ID 0x1AF4
#define IVSHMEM_DEVICE_ID 0x1110


static int ivshmem_probe(struct pci_dev *dev, const struct pci_device_id *id)
{

  unsigned int bar_addr;
  int nvec;
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
  nvec = pci_alloc_irq_vectors(dev, 1, 1, PCI_IRQ_MSI | PCI_IRQ_MSIX);
  if (nvec < 0)
    goto out_release;
  printk(KERN_DEBUG "Successfully allocate %d irq vectors", nvec);
  return 0;


out_release:
  pci_release_regions(dev);
out_disable:
  pci_disable_device(dev);
  return -ENODEV;

}

static void ivshmem_remove(struct pci_dev *dev)
{
  pci_release_regions(dev);
  pci_disable_device(dev);
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
