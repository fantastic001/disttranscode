
##########
# LOCALS #
##########

locals {
  cloud_parameters = merge(var.cloud_parameters, {
    no_comma_params = false
  })
}

############
# PROVIDER #
############

provider "google" {
  project = var.project_id
  region  = var.region
}

##############
# Google API #
##############


module "project_services" {
  source  = "terraform-google-modules/project-factory/google//modules/project_services"
  version = "~> 12.0"

  project_id = var.project_id

  activate_apis = [
    "compute.googleapis.com",
    "iam.googleapis.com",
  ]

  enable_apis                 = true
  disable_services_on_destroy = false
}

#################
# SLURM CLUSTER #
#################

module "slurm_cluster" {
  source = "git@github.com:SchedMD/slurm-gcp.git//terraform/slurm_cluster?ref=v5.0.0"

  cgroup_conf_tpl              = var.cgroup_conf_tpl
  cloud_parameters             = local.cloud_parameters
  cloudsql                     = var.cloudsql
  slurm_cluster_name           = var.slurm_cluster_name
  compute_startup_scripts      = var.compute_startup_scripts
  controller_instance_config   = var.controller_instance_config
  controller_startup_scripts   = var.controller_startup_scripts
  enable_devel                 = var.enable_devel
  enable_bigquery_load         = var.enable_bigquery_load
  enable_cleanup_compute       = var.enable_cleanup_compute
  enable_cleanup_subscriptions = var.enable_cleanup_subscriptions
  enable_reconfigure           = var.enable_reconfigure
  epilog_scripts               = var.epilog_scripts
  login_startup_scripts        = var.login_startup_scripts
  login_network_storage        = var.login_network_storage
  login_nodes                  = var.login_nodes
  network_storage              = var.network_storage
  partitions                   = var.partitions
  project_id                   = var.project_id
  prolog_scripts               = var.prolog_scripts
  slurmdbd_conf_tpl            = var.slurmdbd_conf_tpl
  slurm_conf_tpl               = var.slurm_conf_tpl

  depends_on = [
    # Ensure services are enabled
    module.project_services,
  ]
}
