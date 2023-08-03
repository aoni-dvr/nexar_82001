typedef struct pci_device {
    uint64_t base;
}pci_device_s;

int wrap_nvme_pcicfg_map_bar(struct pci_device *dev, void *addr);
void wrap_nvme_pcicfg_read32(struct pci_device *dev, void *value, uint32_t offset);
void wrap_nvme_pcicfg_write32(struct pci_device *dev, void *value, uint32_t offset);
struct pci_device *wrap_nvme_pci_ctrlr_probe(void);

#define nvme_pcicfg_get_bar_addr_len          // optional for controller memory buffer location
#define nvme_pcicfg_map_bar(a,b,c,d)          wrap_nvme_pcicfg_map_bar(a,d)
#define nvme_pcicfg_map_bar_write_combine     // optional for controller memory buffer location
#define nvme_pcicfg_unmap_bar
#define nvme_pcicfg_read32                    wrap_nvme_pcicfg_read32
#define nvme_pcicfg_write32                   wrap_nvme_pcicfg_write32

#define nvme_pci_ctrlr_probe                  wrap_nvme_pci_ctrlr_probe


