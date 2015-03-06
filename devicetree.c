/*  devicetree.c - Devicetree function example module
 *
 *  Copyright (C) 2015 Bart Tanghe <bart.tanghe@scarlet.be
 *
 */

#include <linux/module.h>  	/* Needed by all modules */
#include <linux/kernel.h>  	/* Needed for KERN_ALERT */
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/of_address.h>
#include <linux/slab.h>

struct dts_example {
	struct device *dev;
	void __iomem *base;
	void __iomem *base_of;
	struct clk *clk;
	const struct of_device_id *match;		//zynq tutorial
	struct device_node *node;
};

static const struct of_device_id dts_example_of_match[] = {
	{ .compatible = "general,dts-example", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, dts_example_of_match);

//static inline struct dts_example *to_dts_example(struct dts_example *chip)
//{
//	return container_of(chip, struct dts_example, chip);
//}
int dts_example_probe(struct platform_device *pdev)
{
	struct dts_example *pc;
	struct resource *res,*res_of;
	const struct property *custom_prop;

	int rc = 0;
	
	
        printk("Devicetree function examples\n");

	pc = devm_kzalloc(&pdev->dev, sizeof(*pc), GFP_KERNEL);
	if (!pc)
		return -ENOMEM;

	pc = devm_kzalloc(&pdev->dev, sizeof(*pc), GFP_KERNEL);
	if (!pc)
		return -ENOMEM;

	res_of = kmalloc(sizeof(struct resource), GFP_KERNEL);
	if (!res_of)
		return -ENOMEM;

	custom_prop = kmalloc(sizeof(struct property), GFP_KERNEL);
	if (!custom_prop)
		return -ENOMEM;

	pc->dev = &pdev->dev;

	pc->match = of_match_device(dts_example_of_match, &pdev->dev);
	pc->node = of_find_matching_node(NULL,pc->match);

	if (!pc->match)
		return -EINVAL;

	//get resources - 2 possibility's

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	printk("resource start %x end %x\n",res->start,res->end);

	pc->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(pc->base))
		return PTR_ERR(pc->base);

	rc = of_address_to_resource(pdev->dev.of_node, 0, res_of);
  	if (rc) {
    		/* Fail */
  	}

  	if  (!request_mem_region(res_of->start, resource_size(res_of), "dts-example")) {
    		/* Fail */
  	}

	printk("resource start %x end %x\n",res_of->start,res_of->end);

  	pc->base_of = of_iomap(pdev->dev.of_node, 0);

  	if (pc->base_of) {
    		/* Fail */
  	}

	//get clk description

	/*pc->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(pc->clk)) {
		dev_err(&pdev->dev, "clock not found: %ld\n", PTR_ERR(pc->clk));
		return PTR_ERR(pc->clk);
	}*/

	custom_prop = of_get_property(pc->node,"custom_var",NULL);

	printk("property name %s\n",custom_prop->name);
	printk("property value %x\n",(int)custom_prop->value);

   	return 0;		// A non 0 return means init_module failed; module can't be loaded.
}

static int dts_example_remove(struct platform_device *pdev)
{
	printk("Remove devicetree function example\n");
	return 0;
}

static struct platform_driver dts_example_driver = {
	.driver = {
		.name = "dts-example",
		.of_match_table = dts_example_of_match,
	},
	.probe = dts_example_probe,
	.remove = dts_example_remove,
};
module_platform_driver(dts_example_driver);

MODULE_LICENSE("GPL");

