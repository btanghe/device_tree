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
	void __iomem *base_node;
	struct clk *clk;
	struct clk *clk_node;
	const struct of_device_id *match;		//zynq tutorial
	struct device_node *node;
};

static const struct of_device_id dts_example_of_match[] = {
	{ .compatible = "general,dts-example", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, dts_example_of_match);

int dts_example_probe(struct platform_device *pdev)
{
	struct dts_example *pc;
	struct resource *res,*res_of,*res_node;
	const void *custom_prop;

	int rc = 0;

        printk("Devicetree function examples\n");

	pc = devm_kzalloc(&pdev->dev, sizeof(*pc), GFP_KERNEL);
	if (!pc)
		return -ENOMEM;

	res_of = kmalloc(sizeof(struct resource), GFP_KERNEL);
	if (!res_of)
		return -ENOMEM;

	res_node = kmalloc(sizeof(struct resource), GFP_KERNEL);
	if (!res_node)
		return -ENOMEM;

	pc->dev = &pdev->dev;

	pc->match = of_match_device(dts_example_of_match, &pdev->dev);
	pc->node = of_find_matching_node(NULL,pc->match);
	if (!pc->match)
		return -EINVAL;
	
	printk("pc->node: '%s' - pdev->dev.of_node: '%s' - pc->node: '%s'\n",pc->node->name,pdev->dev.of_node->name,pc->node->name);

	//get resources - 3 possibility's
	//pdev
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	printk("resource start %x end %x\n",res->start,res->end);

	//pdev->dev.of_node
	rc = of_address_to_resource(pdev->dev.of_node, 0, res_of);
  	if (rc) {
    		printk("of_address_to_resource res_of failed\n");
  	}

	printk("resource start %x end %x\n",res_of->start,res_of->end);

	//pc->node
	rc = of_address_to_resource(pc->node, 0, res_node);
  	if (rc) {
    		printk("of_address_to_resource res_node failed\n");
  	}

	printk("resource start %x end %x\n",res_node->start,res_node->end);

	//ioremap resources
	//pdev

	/*Comment - devm_ioremap_resource doesn't need request_mem_region (is done in the function itself)
	http://lxr.free-electrons.com/source/lib/devres.c#L123*/
	
	pc->base = devm_ioremap_resource(&pdev->dev, res);

	if (IS_ERR(pc->base))
		printk("pdev->dev ioremap failed\n");
	
	//pdev->dev.of_node
  	if  (!request_mem_region(res_of->start, resource_size(res_of), "dts-example")) {
    		printk("request_mem_region res_of failed,already in use\n");
  	}
	else {
	  	pc->base_of = of_iomap(pdev->dev.of_node, 0);

	  	if (IS_ERR(pc->base_of))
			printk("pdev->dev.of_node ioremap failed\n");
	}

	//pc->node
  	if  (!request_mem_region(res_node->start, resource_size(res_node), "dts-example")) {
    		printk("request_mem_region res_node failed,already in use\n");
  	}
	else {
	  	pc->base_node = of_iomap(pc->node, 0);

	  	if (IS_ERR(pc->base_of))
			printk("pc->node ioremap failed\n");
	}

	//get clk description

	pc->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(pc->clk)) {
		return PTR_ERR(pc->clk);
	}
	printk("clock property: %ld\n",clk_get_rate(pc->clk));

	pc->clk_node = of_clk_get(pc->node, 0);
	if (IS_ERR(pc->clk_node)) {
		return PTR_ERR(pc->clk_node);
	}
	printk("clock property: %ld\n",clk_get_rate(pc->clk_node));

	// get custom property description

	custom_prop = of_get_property(pdev->dev.of_node,"custom-var",NULL);

	if (!custom_prop) {
    		printk("no property in dtb\n");
  	}
	else{
		//be32_to_cpup big<->little endian compatible
		//device tree is in big endian format 
		printk("property value %x\n",be32_to_cpup(custom_prop));
	}
	platform_set_drvdata(pdev, pc);
	
   	return 0;		// A non 0 return means init_module failed; module can't be loaded.
}

static int dts_example_remove(struct platform_device *pdev)
{
	struct dts_example *pc;
	
	printk("Remove devicetree function example\n");
	pc  = platform_get_drvdata(pdev);

	//pc will not be used in this remove function

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

