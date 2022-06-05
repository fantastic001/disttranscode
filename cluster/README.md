## Usage

Modify [example.tfvars](./example.tfvars) with required and desired values.

Then perform the following commands on this
[terraform project](../../../../../docs/glossary.md#terraform-project) root
directory:

- `terraform init` to get the plugins
- `terraform validate` to validate the configuration
- `terraform plan -var-file=example.tfvars` to see the infrastructure plan
- `terraform apply -var-file=example.tfvars` to apply the infrastructure build
- `terraform destroy -var-file=example.tfvars` to destroy the built
  infrastructure

## Dependencies

- [slurm_cluster module](../../../../README.md#dependencies)

## Example API

For the terraform example API reference, please see
[README_TF.md](./README_TF.md).
